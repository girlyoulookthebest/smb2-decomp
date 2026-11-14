/* Light groups and stage lighting. */
#include "types.h"
#include "mathutil.h"
#include "light.h"
#include "gx.h"
#include "libc.h"
#include "mode.h"
#include "stage.h"
#include "futsal_data.h"
#include "camera.h"
#include "avdisp.h"

/* Copy the light group to start from, and which parts to reload; one per light group. */
const struct LightGroupInfo lightGroupInfos[LIGHT_GROUP_COUNT] =
{
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_SINGLE, 3 },
    { LIGHT_GROUP_SINGLE, 1 },
    { LIGHT_GROUP_DEFAULT, 1 },
    { LIGHT_GROUP_DEFAULT, 1 },
    { LIGHT_GROUP_DEFAULT, 1 },
    { LIGHT_GROUP_BG, 3 },
    { LIGHT_GROUP_BG, 1 },
    { LIGHT_GROUP_SEL, 3 },
    { LIGHT_GROUP_SINGLE, 1 },
    { LIGHT_GROUP_SINGLE, 1 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
    { LIGHT_GROUP_DEFAULT, 3 },
};

char *lightTypeNames[] =
{
    "INFINITE", "POINT", "SPOT", "POINT_POW", "SPOT_POW",
    "POINT_DARKPOW", "SPOT_DARKPOW", "POINT_DARK", "SPOT_DARK",
};

char *lightIdNames[] =
{
    "TEST", "STAGE", "BUMPER", "AUTO", "BG", "BG_PILLAR", "BG_WALL", "BG_COUNTER", "SEL",
};

char *lightGroupNames[] =
{
    "DEFAULT", "SINGLE", "SINGLE_UNIT", "DEF_MINIMAP", "DEF_GMAT", "DEF_MIR", "BG", "BG_GMAT",
    "SEL", "SHOOT", "SHOOT2",
    "BG_0", "BG_1", "BG_2", "BG_3", "BG_4", "BG_5", "BG_6", "BG_7",
    "BG_8", "BG_9", "BG_a", "BG_b", "BG_c", "BG_d", "BG_e", "BG_f",
};

/* Extra lights for particular stages (by stage ID); lots of empty space at the end. */
struct Light stageLights[512] =
{
    { 1, 3, 1, 1, 0, 150, 1.0f, 1.0f, 1.0f, { 14.500019f, 2.5f, -21.000044f }, 0, 0, 0, { 0.0f, 0.0f, -1.0f }, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f },
    { 1, 3, 1, 3, 0, 134, 0.8f, 1.0f, 0.8f, { 0.09996f, 3.22824f, 0.19996f }, 0, 0, 0, { 0.0f, 0.0f, -1.0f }, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f },
    { 1, 3, 1, 3, 0, 92, 0.8f, 1.0f, 0.8f, { 0.09996f, 3.22824f, 0.19996f }, 0, 0, 0, { 0.0f, 0.0f, -1.0f }, 10.0f, 1.0f, 0.0f, 0.0f, 0.0f },
    { 1, 3, 1, 2, 4, 180, 1.0f, 0.8f, 0.6f, { 0.0f, 23.000051f, 0.0f }, -16768, 24576, 0, { 0.026027f, -0.999322f, -0.026027f }, 0.1f, 0.1f, 0.4f, 0.1f, 40.000076f },
    { 1, 3, 2, 2, 4, 180, 1.0f, 0.8f, 0.6f, { 15.000021f, 23.000051f, 0.0f }, -16768, 24576, 0, { 0.026027f, -0.999322f, -0.026027f }, 0.1f, 0.1f, 0.4f, 0.1f, 40.000076f },
    { 1, 3, 3, 2, 4, 180, 1.0f, 0.8f, 0.6f, { -15.000021f, 23.000051f, 0.0f }, -16768, 24576, 0, { 0.026027f, -0.999322f, -0.026027f }, 0.1f, 0.1f, 0.4f, 0.1f, 40.000076f },
    { 1, 5, 1, 1, 1, 180, 1.0f, 0.5f, 0.0f, { -42.699883f, 31.400082f, -24.599998f }, -8576, 9344, 0, { -5313.705f, -7326.543f, -4252.8f }, 5.700185f, 12.300009f, 1.959999f, 1.709999f, 20.0f },
    { 1, 5, 2, 2, 1, 180, 1.0f, 0.8f, 0.6f, { -35.0f, 37.5f, 25.0f }, 0, 0, 0, { -0.40147f, -1.0f, 0.29441f }, 50.0f, 1.5f, -0.02f, 0.0f, 20.0f },
    { 1, 5, 3, 1, 0, 180, 1.0f, 0.5f, 0.0f, { 50.599762f, 37.5f, -25.0f }, -25600, 28160, 0, { 0.0f, 0.0f, -10000.0f }, 20.10023f, 12.900013f, 0.09f, 0.0f, 15.599983f },
    { 1, 5, 4, 2, 0, 180, 1.0f, 0.8f, 0.0f, { 35.00004f, 37.5f, 25.00006f }, -7680, -30080, 0, { 2290.4126f, -4386.1626f, 8689.971f }, 30.000267f, 1.5f, 0.6f, 0.01f, 25.00002f },
    { 1, 6, 1, 1, 3, 180, 1.0f, 0.5f, 0.0f, { 80.0f, 10.0f, 71.5f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 2, 1, 3, 180, 1.0f, 0.5f, 0.0f, { 40.0f, 10.0f, 71.5f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 3, 1, 3, 180, 1.0f, 0.5f, 0.0f, { 0.0f, 10.0f, 71.5f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 4, 1, 3, 180, 1.0f, 0.5f, 0.0f, { -40.0f, 10.0f, 71.5f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 5, 1, 3, 180, 1.0f, 0.5f, 0.0f, { -80.0f, 10.0f, 71.5f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 6, 1, 3, 180, 1.0f, 0.5f, 0.0f, { -97.5f, 10.0f, 40.0f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 7, 1, 3, 180, 1.0f, 0.5f, 0.0f, { -97.5f, 0.0f, 40.0f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 6, 8, 1, 3, 180, 1.0f, 0.5f, 0.0f, { -97.5f, 10.0f, -40.0f }, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 30.000078f, 0.5f, 0.5f, 0.0f, 0.0f },
    { 1, 7, 1, 2, 1, 180, 1.0f, 0.7f, 0.3f, { 28.25f, 35.0f, -64.0f }, -19328, 0, 0, { 0.0f, -0.960431f, 0.27852f }, 15.000019f, 1.0f, 0.0f, 0.0f, 25.0f },
    { 1, 7, 2, 2, 1, 180, 1.0f, 0.7f, 0.3f, { -26.75f, 35.0f, -64.0f }, -19328, 0, 0, { 0.0f, -0.960431f, 0.27852f }, 15.000019f, 1.0f, 0.0f, 0.0f, 25.0f },
    { -1 },
};


/* .bss, addressed from one base register (order matters). */
struct Light s_lightPool[32];
s32 u_lightToPrint;
s32 u_printLight;
s32 lightingStageId;  /* stage the lights were set up for */
s32 u_lightToCopy;
s32 u_copyLight;
s32 s_lightPoolSize;
GXLightObj s_lightObj;
float s_minimapLightRefDistScale;
s32 g_active_light_group;
struct BgLightInfo s_bgLightInfo;
struct LightGroup s_lightGroups[LIGHT_GROUP_COUNT];
s32 s_lightPerfTimer;
s32 u_someLGIdx;  /* copyFrom of the last loaded group */
s32 lightGroupStack[3];
s8 s_lightGroupStackPos;
s32 lbl_1_bss_6EF0;
s32 lbl_1_bss_6EF4;
s32 lbl_1_bss_6EF8;
s32 s_numLightObjsLoaded;
s32 s_lightGroupsInitialized;
s32 u_drawLightMarker;  /* draw a marker for light u_lightToPrint */

extern struct ThemeLight themeLights[];  /* per-theme lights, terminated by theme == -1 */
extern s8 *s_bilLightGroups[];
extern struct BgLightInfo theme_lights[];             /* one per world theme */
extern struct StageBgLightInfo stageBgLightInfos[];   /* per-stage overrides */
extern s32 stageBgLightInfoCount;

/* Index of the pool light with the given ID and instance; unless findExisting, a free
 * slot is returned when there is none. -1 if nothing fits. */
static inline s8 alloc_pool_light_idx(BOOL findExisting, int id, int inst)
{
    int i;
    struct Light *light;

    for (light = s_lightPool, i = 0; i < s_lightPoolSize; i++, light++)
    {
        if (light->id == id && light->inst == inst)
            return i;
    }
    if (findExisting == TRUE)
        return -1;
    for (light = s_lightPool, i = 0; i < 32; i++, light++)
    {
        if (light->valid == 0)
            return i;
    }
    return -1;
}

/* Unreferenced (stripped at link). Fixes the .bss order, which follows first use, and
 * makes .data the first pooled section referenced, as in the original. */
void light_force_bss_order(void **p)
{
    p[0] = lightTypeNames;
    p[0] = &s_lightPool;
    p[1] = &u_lightToPrint;
    p[2] = &u_printLight;
    p[3] = &lightingStageId;
    p[4] = &u_lightToCopy;
    p[5] = &u_copyLight;
    p[6] = &s_lightPoolSize;
    p[7] = &s_lightObj;
    p[8] = &s_minimapLightRefDistScale;
    p[9] = &g_active_light_group;
    p[10] = &s_bgLightInfo;
    p[11] = &s_lightGroups;
    p[12] = &s_lightPerfTimer;
    p[13] = &u_someLGIdx;
    p[14] = &lightGroupStack;
    p[15] = &s_lightGroupStackPos;
    p[16] = &lbl_1_bss_6EF0;
    p[17] = &lbl_1_bss_6EF4;
    p[18] = &lbl_1_bss_6EF8;
    p[19] = &s_numLightObjsLoaded;
    p[20] = &s_lightGroupsInitialized;
    p[21] = &u_drawLightMarker;
}


void print_light(struct Light *light)
{
    printf("{\t\n");
    printf("\t%d,\tLID_%s,\t%d,\n", light->valid, lightIdNames[light->id], u_lightToPrint);
    printf("\tLTP_%s,\t%d,\t%d,\n", lightTypeNames[light->type], light->spotFn, current_stage_id);
    printf("\t{ %f, %f, %f },\n", light->red, light->green, light->blue);
    printf("\t{ %f, %f, %f },\n", light->pos.x, light->pos.y, light->pos.z);
    printf("\t0x%x,\t0x%x,\t0x%x,\t{ %f, %f, %f },\n", light->rotX, light->rotY, light->rotZ,
           light->dir.x, light->dir.y, light->dir.z);
    printf("\t%f,\t%f,\t%f,\n", light->refDist, light->k0, light->k1);
    printf("\t%f,\t%f\n", light->k2, light->spotCutoff);
    printf("},\n\n");
}

void init_light_dir(struct Light *light)
{
    switch (light->type)
    {
    case LIGHT_TYPE_INFINITE:
        light->dir.x = light->dir.y = 0.0f;
        light->dir.z = -1.0f;
        mtxa_from_identity();
        mtxa_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        mtxa_tf_vec(&light->dir, &light->dir);
        light->dir.x *= 10000.0f;
        light->dir.y *= 10000.0f;
        light->dir.z *= 10000.0f;
        break;
    case LIGHT_TYPE_SPOT:
    case LIGHT_TYPE_SPOT_POW:
    case LIGHT_TYPE_SPOT_DARKPOW:
    case LIGHT_TYPE_SPOT_DARK:
        if (light->id != LIGHT_ID_BG_PILLAR)
        {
            light->dir.x = light->dir.y = 0.0f;
            light->dir.z = -1.0f;
            mtxa_from_identity();
            mtxa_rotate_y(light->rotY);
            mtxa_rotate_x(light->rotX);
            mtxa_tf_vec(&light->dir, &light->dir);
        }
        break;
    }
}

/* Build the GX light object for a light and load it into hardware light slot
 * lightIdxInGroup, keeping a copy in the group. */
void load_light(struct LightGroup *group, int lightIdxInGroup, struct Light *light)
{
    GXColor lightColor;
    Vec tempVec;
    float refDist;
    float yOffset;

    lightColor.r = light->red * 255.0f;
    lightColor.g = light->green * 255.0f;
    lightColor.b = light->blue * 255.0f;
    lightColor.a = 255;

    switch (light->type)
    {
    case LIGHT_TYPE_INFINITE:
    case LIGHT_TYPE_POINT:
    case LIGHT_TYPE_POINT_POW:
    case LIGHT_TYPE_POINT_DARKPOW:
    case LIGHT_TYPE_POINT_DARK:
        GXInitLightSpot(&s_lightObj, 0.0f, GX_SP_OFF);
        break;
    case LIGHT_TYPE_SPOT:
    case LIGHT_TYPE_SPOT_POW:
    case LIGHT_TYPE_SPOT_DARKPOW:
    case LIGHT_TYPE_SPOT_DARK:
        GXInitLightSpot(&s_lightObj, light->spotCutoff, light->spotFn);
        break;
    }

    refDist = (g_active_light_group != LIGHT_GROUP_DEF_MINIMAP)
            ? light->refDist
            : light->refDist * s_minimapLightRefDistScale;
    switch (light->type)
    {
    case LIGHT_TYPE_INFINITE:
    case LIGHT_TYPE_POINT:
    case LIGHT_TYPE_SPOT:
        GXInitLightDistAttn(&s_lightObj, refDist * light->k0, light->k1 + 0.05f,
                            (light->type == LIGHT_TYPE_INFINITE) ? 0 : (int)(light->k2 * 100.0) + 3);
        break;
    case LIGHT_TYPE_POINT_POW:
    case LIGHT_TYPE_SPOT_POW:
        GXInitLightAttnK(&s_lightObj, light->k0 * (-85.0f * (1.0f / (refDist * refDist))), light->k1,
                         light->k2 + (0.95f / (0.05f * (refDist * refDist))));
        break;
    case LIGHT_TYPE_POINT_DARKPOW:
    case LIGHT_TYPE_SPOT_DARKPOW:
        GXInitLightAttnK(&s_lightObj, light->k0, light->k1,
                         (-0.1f + light->k2) * (0.95f / (0.05f * (refDist * refDist))));
        break;
    case LIGHT_TYPE_POINT_DARK:
    case LIGHT_TYPE_SPOT_DARK:
        GXInitLightAttnK(&s_lightObj, -1.0f + light->k0, light->k1,
                         (-0.1f + light->k2) * (0.95f / (0.05f * (refDist * refDist))));
        break;
    }

    GXInitLightColor(&s_lightObj, lightColor);

    yOffset = (g_active_light_group != LIGHT_GROUP_DEF_MINIMAP)
            ? 0.0
            : 2.05f * ((1.0f / s_minimapLightRefDistScale) - 1.0f);
    tempVec.x = light->pos.x;
    tempVec.y = light->pos.y + yOffset;
    tempVec.z = light->pos.z;
    switch (light->type)
    {
    case LIGHT_TYPE_INFINITE:
        mtxa_tf_point(&light->dir, &tempVec);
        GXInitLightPos(&s_lightObj, tempVec.x, tempVec.y, tempVec.z);
        break;
    case LIGHT_TYPE_POINT:
    case LIGHT_TYPE_POINT_POW:
    case LIGHT_TYPE_POINT_DARKPOW:
    case LIGHT_TYPE_POINT_DARK:
        mtxa_tf_point(&tempVec, &tempVec);
        GXInitLightPos(&s_lightObj, tempVec.x, tempVec.y, tempVec.z);
        break;
    case LIGHT_TYPE_SPOT:
    case LIGHT_TYPE_SPOT_POW:
    case LIGHT_TYPE_SPOT_DARKPOW:
    case LIGHT_TYPE_SPOT_DARK:
        mtxa_tf_point(&tempVec, &tempVec);
        GXInitLightPos(&s_lightObj, tempVec.x, tempVec.y, tempVec.z);
        mtxa_tf_vec(&light->dir, &tempVec);
        GXInitLightDir(&s_lightObj, tempVec.x, tempVec.y, tempVec.z);
        break;
    }
    GXLoadLightObjImm(&s_lightObj, 1 << lightIdxInGroup);
    memcpy_handler(&group->lightObjs[lightIdxInGroup], &s_lightObj, sizeof(group->lightObjs[lightIdxInGroup]));
}

/* Pick the background lighting for the stage and add its infinite light to the pool. */
void init_bg_lighting(int stageId)
{
    struct Light light;
    int i;

    s_bgLightInfo = theme_lights[(world_theme < 0 || stageId == 0) ? 0 : world_theme];
    for (i = 0; i < stageBgLightInfoCount; i++)
    {
        if (stageId == stageBgLightInfos[i].stageId)
        {
            s_bgLightInfo = stageBgLightInfos[i].info;
            break;
        }
    }
    if (s_bgLightInfo.infLightColor.r == 0.0
     && s_bgLightInfo.infLightColor.g == 0.0
     && s_bgLightInfo.infLightColor.b == 0.0)
        return;

    if (stageId != 0)
    {
        if (sub_mode == SMD_GAME_CONTINUE_INIT || sub_mode == SMD_GAME_CONTINUE_MAIN)
        {
            s_bgLightInfo.infLightRotX = 0;
            s_bgLightInfo.infLightRotY = 0x4000;
        }
        if (world_theme == 3 && modeCtrl.unk30 > 1)
            set_bg_ambient(0.4f, 0.6f, 0.9f);
        if (world_theme == 16)
        {
            switch (modeCtrl.difficulty)
            {
            case 0:
                s_bgLightInfo.infLightRotY += 45056;
                break;
            }
        }
    }
    memset(&light, 0, sizeof(light));
    light.id = LIGHT_ID_STAGE;
    light.inst = 0;
    light.type = LIGHT_TYPE_INFINITE;
    light.red = s_bgLightInfo.infLightColor.r;
    light.green = s_bgLightInfo.infLightColor.g;
    light.blue = s_bgLightInfo.infLightColor.b;
    light.rotX = s_bgLightInfo.infLightRotX;
    light.rotY = s_bgLightInfo.infLightRotY;
    add_light_to_pool(&light);
}

void init_light_groups(void)
{
    int i;
    int lightInGroupIdx;
    struct Light *light;
    const struct LightGroupInfo *lgInfo;
    struct LightGroup *lightGrp;

    lightGrp = s_lightGroups;
    for (i = 0; i < LIGHT_GROUP_COUNT; i++, lightGrp++)
    {
        for (lightInGroupIdx = 0; lightInGroupIdx < 8; lightInGroupIdx++)
            lightGrp->lightPoolIdxs[lightInGroupIdx] = -1;
        lightGrp->ambient.r = s_bgLightInfo.ambient.r;
        lightGrp->ambient.g = s_bgLightInfo.ambient.g;
        lightGrp->ambient.b = s_bgLightInfo.ambient.b;
    }

    /* The default group gets the background's infinite light plus per-stage lights. */
    light = s_lightPool;
    lightInGroupIdx = 0;
    for (i = 0; i < 32; i++, light++)
    {
        if (light->valid != 0 && (light->id == LIGHT_ID_STAGE || light->id == LIGHT_ID_TEST || light->id == LIGHT_ID_AUTO))
        {
            s_lightGroups[LIGHT_GROUP_DEFAULT].lightPoolIdxs[lightInGroupIdx] = i;
            if (++lightInGroupIdx == 8)
                break;
        }
    }

    if (s_lightPool[0].valid != 0)
        s_lightGroups[LIGHT_GROUP_SINGLE].lightPoolIdxs[0] = 0;

    light = s_lightPool;
    lightInGroupIdx = 0;
    for (i = 0; i < 32; i++, light++)
    {
        if (light->valid != 0 && (light->id == LIGHT_ID_BG || light->id == LIGHT_ID_TEST || light->id == LIGHT_ID_AUTO))
        {
            s_lightGroups[LIGHT_GROUP_BG].lightPoolIdxs[lightInGroupIdx] = i;
            if (++lightInGroupIdx == 8)
                break;
        }
    }

    light = s_lightPool;
    lightInGroupIdx = 0;
    for (i = 0; i < 32; i++, light++)
    {
        if (light->valid != 0 && light->id == LIGHT_ID_SEL)
        {
            s_lightGroups[LIGHT_GROUP_SEL].lightPoolIdxs[lightInGroupIdx] = i;
            if (++lightInGroupIdx == 8)
                break;
        }
    }

    /* Groups 2-10 start as copies of another group. */
    lightGrp = &s_lightGroups[LIGHT_GROUP_SINGLE_UNIT];
    lgInfo = &lightGroupInfos[LIGHT_GROUP_SINGLE_UNIT];
    for (i = LIGHT_GROUP_SINGLE_UNIT; i < LIGHT_GROUP_BG_0; i++, lightGrp++, lgInfo++)
        memcpy_handler(&s_lightGroups[i], &s_lightGroups[lgInfo->copyFrom], sizeof(s_lightGroups[i]));

    if (s_bgLightInfo.bgLightGroups == NULL)
        return;

    lightGrp = &s_lightGroups[LIGHT_GROUP_BG_1];
    for (i = LIGHT_GROUP_BG_1; i < LIGHT_GROUP_COUNT; i++, lightGrp++)
    {
        int bgLgIdx = i - LIGHT_GROUP_BG_1;

        if (s_bgLightInfo.bgLightGroups[bgLgIdx] == NULL)
            break;
        for (lightInGroupIdx = 0; lightInGroupIdx < 8; lightInGroupIdx++)
        {
            if (s_bgLightInfo.bgLightGroups[bgLgIdx][lightInGroupIdx * 2 + 0] == -1)
                break;
            lightGrp->lightPoolIdxs[lightInGroupIdx] = alloc_pool_light_idx(TRUE,
                s_bgLightInfo.bgLightGroups[bgLgIdx][lightInGroupIdx * 2 + 0],
                s_bgLightInfo.bgLightGroups[bgLgIdx][lightInGroupIdx * 2 + 1]);
        }
    }
}

/* Pass the group's infinite light (the first light, if it is one) on to avdisp; without
 * one, avdisp gets a fixed white light when the game isn't paused, else none. */
void set_avdisp_inf_light(struct LightGroup *lightGrp)
{
    Vec infLightDir;
    GXColor infLightColor;
    int hasInfLight = TRUE;

    if (lightGrp->lightPoolIdxs[0] == -1)
        hasInfLight = FALSE;
    else
    {
        struct Light *light = &s_lightPool[lightGrp->lightPoolIdxs[0]];

        if (light->valid == 0 || light->type != LIGHT_TYPE_INFINITE)
            hasInfLight = FALSE;
    }

    if (hasInfLight)
    {
        /* For directional lights the light position is the direction. */
        GXGetLightPos(&lightGrp->lightObjs[0], &infLightDir.x, &infLightDir.y, &infLightDir.z);
        GXGetLightColor(&lightGrp->lightObjs[0], &infLightColor);
        fn_1_10A3A0(&infLightDir);
        fn_1_10A410(infLightColor.r / 255.0f, infLightColor.g / 255.0f, infLightColor.b / 255.0f);
    }
    else if (g_check_flag_related_to_pausing() != 0)
    {
        Vec dir = { 0.0f, 1.0f, -0.5f };

        mtxa_push();
        mtxa_from_rotate_y(g_current_camera->rotY);
        mtxa_tf_vec(&dir, &dir);
        mtxa_pop();
        fn_1_10A3A0(&dir);
        fn_1_10A410(1.0f, 1.0f, 1.0f);
    }
    else
        fn_1_10A410(0.0f, 0.0f, 0.0f);
}

/* Load a dark, zeroed light into all eight hardware light slots. */
void g_init_lights(void)
{
    GXColor color = { 0, 0, 0, 0 };
    int i;

    memset(&s_lightObj, 0, sizeof(s_lightObj));
    GXInitLightSpot(&s_lightObj, 0.0f, GX_SP_OFF);
    GXInitLightDistAttn(&s_lightObj, 0.0f, 0.0f, 0);
    GXInitLightColor(&s_lightObj, color);
    GXInitLightPos(&s_lightObj, 0.0f, 0.0f, 0.0f);
    GXInitLightDir(&s_lightObj, 0.0f, 0.0f, -1.0f);
    for (i = 1; i < 0x100; i <<= 1)
        GXLoadLightObjImm(&s_lightObj, i);
}

static inline void u_alloc_stage_lights(int stageId)
{
    struct Light *light = stageLights;

    while (light->valid != -1)
    {
        if (light->stageId == stageId)
            add_light_to_pool(light);
        light++;
    }
}

static inline void u_alloc_theme_lights(int theme)
{
    struct ThemeLight *themeLight = themeLights;

    while (themeLight->theme != -1)
    {
        if (themeLight->theme == theme)
            add_light_to_pool(&themeLight->light);
        themeLight++;
    }
}

/* Light system init for a stage: reset the pool, then add the background, stage and
 * theme lights. */
void light_init(int stageId)
{
    int i;
    struct Light *light;

    light = s_lightPool;
    for (i = 0; i < 32; i++, light++)
    {
        memset(light, 0, sizeof(*light));
        light->valid = 0;
        light->id = LIGHT_ID_TEST;
        light->inst = 0;
        light->type = LIGHT_TYPE_POINT;
        light->refDist = 2.0f;
        light->k0 = 1.0f;
        light->k1 = 0.0f;
        light->k2 = 0.0f;
        light->spotFn = GX_SP_COS2;
        light->spotCutoff = 45.0f;
    }
    s_lightPoolSize = 0;

    init_bg_lighting(stageId);

    u_alloc_stage_lights(stageId);
    u_alloc_theme_lights(world_theme);
    lightingStageId = (stageId == 0) ? current_stage_id : stageId;
}

/* Light system per-frame update.
 * NONMATCHING: the .bss and .data base addresses land in r30/r29 instead of r29/r30;
 * the instructions are otherwise identical. */
void light_main(void)
{
    int i;
    struct Light *light;
    struct BgLightInfo *bgInfo;

    s_lightPerfTimer = 0;
    bgInfo = &s_bgLightInfo;
    fn_1_7FF4(bgInfo->unk14, bgInfo->unk18, bgInfo->unk1C);
    fade_color_base_default();
    s_lightPoolSize = 0;

    light = s_lightPool;
    for (i = 0; i < 32; i++, light++)
    {
        if (light->valid != 0)
        {
            init_light_dir(light);
            s_lightPoolSize = i + 1;
        }
    }
    g_active_light_group = -1;
    u_someLGIdx = -1;
    for (i = 0; i < 3; i++)
        lightGroupStack[i] = -1;
    s_lightGroupStackPos = 0;
    s_minimapLightRefDistScale = 1.0f;
    lbl_1_bss_6EF0 = 0;
    lbl_1_bss_6EF4 = 0;
    lbl_1_bss_6EF8 = 0;
    s_numLightObjsLoaded = 0;
    if (s_lightGroupsInitialized == 0)
        init_light_groups();
    mtxa_from_mtxb();
    g_set_active_light_group(LIGHT_GROUP_DEFAULT);
    if (u_printLight != 0)
    {
        print_light(&s_lightPool[u_lightToPrint]);
        u_printLight = 0;
    }
    if (u_copyLight != 0)
    {
        if (lightingStageId == current_stage_id)
            memcpy_handler(&s_lightPool[u_lightToPrint], &s_lightPool[u_lightToCopy], sizeof(s_lightPool[u_lightToPrint]));
        else
            u_alloc_stage_lights(lightingStageId);
        u_copyLight = 0;
    }
}

/* Copy a light into the pool (into the slot of a light with the same ID and instance, or
 * a free one). Returns FALSE when the pool is full. */
BOOL add_light_to_pool(struct Light *light)
{
    int idx = alloc_pool_light_idx(FALSE, light->id, light->inst);
    struct Light *poolLight;

    if (idx == -1)
        return FALSE;
    poolLight = &s_lightPool[idx];
    memcpy_handler(poolLight, light, sizeof(*poolLight));
    poolLight->valid = 1;
    poolLight->k0 = (light->k0 == 0.0f) ? 1.0f : light->k0;
    poolLight->k1 = (light->k1 == 0.0f) ? 0.0f : light->k1;
    poolLight->k2 = (light->k2 == 0.0f) ? 0.0f : light->k2;
    s_lightPoolSize = idx + 1;
    return TRUE;
}

/* The pool light with the given ID and instance, or NULL. */
struct Light *alloc_pool_light(int id, int inst)
{
    id = alloc_pool_light_idx(TRUE, id, inst);
    return (id == -1) ? NULL : &s_lightPool[id];
}

/* Make a light group current: build and load its lights (cached in the group) and set
 * the renderers' light mask, ambient and infinite light. */
void g_set_active_light_group(int lightGrpId)
{
    int i;
    struct LightGroup *lightGrp;
    const struct LightGroupInfo *info;

    perf_init_timer(0);
    lbl_1_bss_6EF0++;
    g_active_light_group = lightGrpId;
    lightGrp = &s_lightGroups[g_active_light_group];
    info = &lightGroupInfos[g_active_light_group];
    mtxa_to_mtx(lightGrp->viewFromWorld);
    if (info->copyFrom != u_someLGIdx || (info->flags & 1) != 0)
    {
        lightGrp->lightMask = 0;
        for (i = 0; i < 8; i++)
        {
            if (lightGrp->lightPoolIdxs[i] != -1)
            {
                struct Light *light = &s_lightPool[lightGrp->lightPoolIdxs[i]];

                if (light->valid != 0)
                {
                    load_light(lightGrp, i, light);
                    lightGrp->lightMask |= 1 << i;
                    lbl_1_bss_6EF4++;
                }
            }
        }
        nl2ngc_set_light_mask(lightGrp->lightMask);
        fn_1_10A380(lightGrp->lightMask);
    }
    if (info->copyFrom != u_someLGIdx || (info->flags & 2) != 0)
        g_smth_with_avdisp_globals(lightGrp->ambient.r, lightGrp->ambient.g, lightGrp->ambient.b);
    set_avdisp_inf_light(lightGrp);
    u_someLGIdx = info->copyFrom;
    s_lightPerfTimer += perf_stop_timer(0);
}

int get_active_light_group(void)
{
    return g_active_light_group;
}

/* Make a light group current using the GX light objects cached in it. */
void load_light_group_cached(int lightGrpId)
{
    struct LightGroup *lightGrp;
    int i;
    const struct LightGroupInfo *info;

    perf_init_timer(0);
    lbl_1_bss_6EF8++;
    g_active_light_group = lightGrpId;
    lightGrp = &s_lightGroups[g_active_light_group];
    info = &lightGroupInfos[g_active_light_group];
    if (info->copyFrom != u_someLGIdx || (info->flags & 1) != 0)
    {
        for (i = 0; i < 8; i++)
        {
            if (lightGrp->lightMask & (1 << i))
            {
                GXLoadLightObjImm(&lightGrp->lightObjs[i], 1 << i);
                s_numLightObjsLoaded++;
            }
        }
        nl2ngc_set_light_mask(lightGrp->lightMask);
        fn_1_10A380(lightGrp->lightMask);
    }
    set_avdisp_inf_light(lightGrp);
    u_someLGIdx = info->copyFrom;
    s_lightPerfTimer += perf_stop_timer(0);
}

void push_light_group(void)
{
    if (s_lightGroupStackPos == 3)
        printf("LIGHT ERROR!!! PushLightGroup() stack over.\n");
    else
    {
        lightGroupStack[s_lightGroupStackPos] = g_active_light_group;
        s_lightGroupStackPos++;
    }
}

void pop_light_group(void)
{
    if (s_lightGroupStackPos == 0)
        printf("LIGHT ERROR!!! PopLightGroup() stack null.\n");
    else
    {
        s_lightGroupStackPos--;
        mtxa_push();
        mtxa_from_mtx(s_lightGroups[lightGroupStack[s_lightGroupStackPos]].viewFromWorld);
        g_set_active_light_group(lightGroupStack[s_lightGroupStackPos]);
        mtxa_pop();
    }
}

void u_reset_light_group_stack(int unused)
{
    g_active_light_group = -1;
    u_someLGIdx = -1;
    mtxa_from_mtxb();
    g_set_active_light_group(LIGHT_GROUP_DEFAULT);
}

void u_set_some_minimap_light_param(float scale)
{
    s_minimapLightRefDistScale = scale;
}

void g_set_light_color_maybe(int lightGrpId, float r, float g, float b)
{
    struct LightGroup *lightGrp = &s_lightGroups[lightGrpId];

    lightGrp->ambient.r = r;
    lightGrp->ambient.g = g;
    lightGrp->ambient.b = b;
}

void set_bg_ambient(float r, float g, float b)
{
    s_bgLightInfo.ambient.r = r;
    s_bgLightInfo.ambient.g = g;
    s_bgLightInfo.ambient.b = b;
}

/* set_render_ambient */
void g_smth_with_avdisp_globals(float r, float g, float b)
{
    nl2ngc_set_ambient(r, g, b);
    call_g_avdisp_set_ambient(r, g, b);
}

void get_curr_light_group_ambient(struct Color3f *color)
{
    *color = s_lightGroups[g_active_light_group].ambient;
}

void apply_curr_light_group_ambient(void)
{
    struct Color3f *ambient = &s_lightGroups[g_active_light_group].ambient;

    nl2ngc_set_ambient(ambient->r, ambient->g, ambient->b);
    call_g_avdisp_set_ambient(ambient->r, ambient->g, ambient->b);
}

/* Debug: draw the light being edited (u_lightToPrint) as a ball sized to its reference
 * distance, and/or a spotlight model pointing along it. */
void u_draw_naomi_ball(void)
{
    struct Light *light;

    if (u_drawLightMarker == 0)
        return;
    light = &s_lightPool[u_lightToPrint];
    switch (light->type)
    {
    case LIGHT_TYPE_POINT:
    case LIGHT_TYPE_POINT_POW:
    case LIGHT_TYPE_POINT_DARKPOW:
    case LIGHT_TYPE_POINT_DARK:
        set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
        mtxa_from_mtxb();
        mtxa_translate(&light->pos);
        mtxa_scale_s(light->refDist * 2.0);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(light->refDist * 2.0);
        avdisp_set_alpha(0.5f);
        avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[30].model);
        mtxa_from_mtxb();
        mtxa_translate(&light->pos);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[30].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        break;
    case LIGHT_TYPE_INFINITE:
        mtxa_from_mtxb();
        mtxa_translate(&g_current_camera->lookAt);
        mtxa_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        mtxa_rotate_x(0x8000);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[104].model);
        break;
    case LIGHT_TYPE_SPOT:
    case LIGHT_TYPE_SPOT_POW:
    case LIGHT_TYPE_SPOT_DARKPOW:
    case LIGHT_TYPE_SPOT_DARK:
        set_post_mult_color(1.0f, 0.0f, 0.0f, 1.0f);
        mtxa_from_mtxb();
        mtxa_translate(&light->pos);
        mtxa_scale_s(light->refDist * 2.0);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_set_scale_factor(light->refDist * 2.0);
        avdisp_set_alpha(0.5f);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[30].model);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
        mtxa_from_mtxb();
        mtxa_translate(&light->pos);
        mtxa_rotate_y(light->rotY);
        mtxa_rotate_x(light->rotX);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[104].model);
        break;
    }
}

/* Background light parameters saved by u_smth_with_lights_smd_continue. */
struct
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
} lbl_1_bss_6F08;

/* Background lighting for the continue screen. */
void u_smth_with_lights_smd_continue(int stageId)
{
    struct BgLightInfo *bgInfo;

    init_bg_lighting(stageId);
    bgInfo = &s_bgLightInfo;
    lbl_1_bss_6F08.unk0 = bgInfo->unk10;
    lbl_1_bss_6F08.unk4 = bgInfo->unk14;
    lbl_1_bss_6F08.unk8 = bgInfo->unk18;
    lbl_1_bss_6F08.unkC = bgInfo->unk1C;
    bgInfo->unk10 = 0.0f;
    bgInfo->unk14 = 0.7f;
    bgInfo->unk18 = 0.7f;
    bgInfo->unk1C = 0.7f;
}

u8 lbl_1_data_10F04[4] = { 0 };

/* Monkey Billiards light groups: (light ID, instance) pairs, ended by -1. */
s8 s_bilLightGroup_BG_1[10] = { 5, 1, 5, 2, 5, 3, 5, 4, -1, -1 };
s8 s_bilLightGroup_BG_2[18] = { 6, 1, 6, 2, 6, 3, 6, 4, 6, 5, 6, 6, 6, 7, 6, 8, -1, -1 };
s8 s_bilLightGroup_BG_3[8] = { 6, 2, 6, 3, 6, 4, -1, -1 };
s8 s_bilLightGroup_BG_4[6] = { 7, 1, 7, 2, -1, -1 };
s8 s_bilLightGroup_BG_5[10] = { 7, 1, 7, 2, 5, 1, 5, 3, -1, -1 };
s8 s_bilLightGroup_BG_6[6] = { 5, 1, 6, 8, -1, -1 };
s8 s_bilLightGroup_BG_7[4] = { 5, 3, -1, -1 };
s8 s_bilLightGroup_BG_8[10] = { 5, 4, 6, 1, 6, 2, 6, 3, -1, -1 };

s8 *s_bilLightGroups[] =
{
    s_bilLightGroup_BG_1, s_bilLightGroup_BG_2, s_bilLightGroup_BG_3,
    s_bilLightGroup_BG_4, s_bilLightGroup_BG_5, s_bilLightGroup_BG_6,
    s_bilLightGroup_BG_7, s_bilLightGroup_BG_8, NULL,
};

/* Monkey Billiards model names and the light group each uses. */
struct BilLightGroupName s_bilLightGroupNames[] =
{
    { "BILL_FLOOR", 1 },
    { "BILL_WALL", 2 },
    { "BILL_PILLAR00", 1 },
    { "BILL_BOX", 3 },
    { "BILL_BAR_COUNTER", 4 },
    { "BILL_CHAIR00", 5 },
    { "BILL_PLANT00", 6 },
    { "BILL_PLANT01", 7 },
    { "BILL_PLANT02", 8 },
    { NULL, -1 },
};

struct ThemeLight themeLights[] =
{
    { 17, { 1, 4, 1, 0, 0, 1, 0.55f, 0.4f, 0.4f, { 0.0f, 0.0f, 0.0f }, -14336, 21760, 0, { -1697.4554f, -9807.853f, 961.5967f }, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 23, { 1, 4, 0, 0, 0, 1, 0.75f, 1.0f, 0.9f, { 0.0f, 0.0f, 0.0f }, -20608, 0, 0, { -1697.4554f, -9807.853f, 961.5967f }, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 26, { 1, 4, 0, 0, 0, 1, 1.0f, 0.1f, 1.0f, { 0.0f, 0.0f, 0.0f }, 8192, 24576, 0, { -1697.4554f, -9807.853f, 961.5967f }, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 27, { 1, 1, 1, 0, 3, 101, 0.06f, 0.06f, 0.08f, { 0.0f, 0.0f, 0.0f }, 2560, 0, 0, { -3598.9504f, 0.0f, -9329.928f }, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f } },
    { 34, { 1, 4, 1, 0, 3, 101, 0.8f, 0.8f, 0.8f, { 0.0f, 0.0f, 0.0f }, 20096, 28416, 0, { -3598.9504f, 0.0f, -9329.928f }, 2.0f, 1.0f, 0.0f, 0.0f, 45.0f } },
    { -1 },
};

struct BgLightInfo theme_lights[] =
{
    { 0.8f, { 0.8f, 0.8f, 0.8f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.28f, 0.48f, 0.63f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.6f, 0.85f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.7f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.7f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.3f, { 0.5f, 0.45f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 29184, 17664, NULL },
    { 0.6f, { 0.45f, 0.4f, 0.25f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 24576, 24576, NULL },
    { 0.6f, { 0.55f, 0.6f, 0.85f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.3f, 0.3f, 0.45f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.7f, 0.8f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.8f, 0.8f, 0.8f }, -11776, 21888, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.55f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.0f, 0.0f, 0.0f }, 8192, 24576, s_bilLightGroups },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 4096, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.25f, 0.25f, 0.35f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.55f, 0.4f, 0.4f }, 12288, 21760, NULL },
    { 0.6f, { 0.3f, 0.4f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.48f, 0.6f, 1.0f }, 8576, 29952, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.5f, 0.5f, 0.5f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 0.9f, 0.8f }, 8576, 9216, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.4f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.8f, 0.8f, 0.8f }, 20096, 28416, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.45f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.6f, 0.55f, 0.7f }, 28672, 21760, NULL },
    { 0.6f, { 0.3f, 0.6f, 0.55f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.75f, 1.0f, 0.9f }, 8192, 24576, NULL },
    { 0.6f, { 0.55f, 0.5f, 0.7f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 0.7f, 0.0f }, 2304, 28928, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, -22016, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.4f, 0.4f, 0.4f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.88f, 0.88f, 0.84f }, 9856, -21888, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 4096, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 27008, 6400, NULL },
    { 0.6f, { 0.56f, 0.54f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 0.96f }, 10240, 30720, NULL },
    { 0.6f, { 0.25f, 0.3f, 0.4f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.55f, 0.7f, 0.8f }, 22016, 31872, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 5120, 26368, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 20096, 28416, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, -22016, NULL },
    { 0.6f, { 0.5f, 0.5f, 0.5f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 0.9f, 0.8f }, 8576, 9216, NULL },
    { 0.6f, { 0.55f, 0.5f, 0.7f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 0.7f, 0.0f }, 2304, 28928, NULL },
    { 0.6f, { 0.3f, 0.4f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 0.48f, 0.6f, 1.0f }, 8576, 29952, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 4096, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
    { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 8192, 24576, NULL },
};

struct StageBgLightInfo stageBgLightInfos[] =
{
    { 200, { 0.6f, { 0.6f, 0.6f, 0.6f }, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, { 1.0f, 1.0f, 1.0f }, 7680, 28800, NULL } },
};

s32 stageBgLightInfoCount = 1;
