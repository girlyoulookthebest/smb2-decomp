#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"
#include "mathutil.h"
#include "gx.h"

struct Light
{
    /*0x00*/ s8 valid;    /* 1 = in use; -1 terminates a table */
    /*0x01*/ s8 id;       /* LIGHT_ID_* category */
    /*0x02*/ s16 inst;    /* instance number within the category */
    /*0x04*/ s8 type;     /* LIGHT_TYPE_* */
    /*0x06*/ s16 spotFn;  /* GXSpotFn */
    /*0x08*/ u8 stageId;
    /*0x0C*/ float red;
    /*0x10*/ float green;
    /*0x14*/ float blue;
    /*0x18*/ Vec pos;
    /*0x24*/ s16 rotX;
    /*0x26*/ s16 rotY;
    /*0x28*/ s16 rotZ;
    /*0x2C*/ Vec dir;
    /*0x38*/ float refDist;
    /*0x3C*/ float k0;
    /*0x40*/ float k1;
    /*0x44*/ float k2;
    /*0x48*/ float spotCutoff;
};  /* size = 0x4C */

enum
{
    LIGHT_TYPE_INFINITE,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
    LIGHT_TYPE_POINT_POW,
    LIGHT_TYPE_SPOT_POW,
    LIGHT_TYPE_POINT_DARKPOW,
    LIGHT_TYPE_SPOT_DARKPOW,
    LIGHT_TYPE_POINT_DARK,
    LIGHT_TYPE_SPOT_DARK,
};

enum
{
    LIGHT_ID_TEST,
    LIGHT_ID_STAGE,  /* the background's infinite light */
    LIGHT_ID_BUMPER,
    LIGHT_ID_AUTO,   /* per-stage light added to the default group */
    LIGHT_ID_BG,
    LIGHT_ID_BG_PILLAR,
    LIGHT_ID_BG_WALL,
    LIGHT_ID_BG_COUNTER,
    LIGHT_ID_SEL,
};

enum
{
    LIGHT_GROUP_DEFAULT,
    LIGHT_GROUP_SINGLE,
    LIGHT_GROUP_SINGLE_UNIT,
    LIGHT_GROUP_DEF_MINIMAP,
    LIGHT_GROUP_DEF_GMAT,
    LIGHT_GROUP_DEF_MIR,
    LIGHT_GROUP_BG,
    LIGHT_GROUP_BG_GMAT,
    LIGHT_GROUP_SEL,
    LIGHT_GROUP_SHOOT,
    LIGHT_GROUP_SHOOT2,
    LIGHT_GROUP_BG_0,
    LIGHT_GROUP_BG_1,
    LIGHT_GROUP_COUNT = LIGHT_GROUP_BG_0 + 16,
};

struct Color3f
{
    float r, g, b;
};

/* Lighting set up for a world theme (or overridden per stage). */
struct BgLightInfo
{
    /*0x00*/ float unk0;
    /*0x04*/ struct Color3f ambient;
    /*0x10*/ float unk10;
    /*0x14*/ float unk14;
    /*0x18*/ float unk18;
    /*0x1C*/ float unk1C;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ float unk28;
    /*0x2C*/ float unk2C;
    /*0x30*/ float unk30;
    /*0x34*/ struct Color3f infLightColor;  /* the global directional ("infinite") light */
    /*0x40*/ s16 infLightRotX;
    /*0x42*/ s16 infLightRotY;
    /*0x44*/ s8 **bgLightGroups;
};  /* size = 0x48 */

struct StageBgLightInfo
{
    /*0x00*/ s32 stageId;
    /*0x04*/ struct BgLightInfo info;
};  /* size = 0x4C */

/* An extra light used whenever a world theme is active. */
struct ThemeLight
{
    /*0x00*/ s32 theme;  /* -1 terminates the table */
    /*0x04*/ struct Light light;
};  /* size = 0x50 */

struct BilLightGroupName
{
    char *name;
    s8 lightGroup;
};

struct LightGroup
{
    /*0x00*/ s16 lightPoolIdxs[8];
    u8 unk10[4];
    /*0x014*/ GXLightObj lightObjs[8];
    /*0x214*/ s32 lightMask;
    /*0x218*/ struct Color3f ambient;
    /*0x224*/ Mtx viewFromWorld;
    u8 unk254[4];
};  /* size = 0x258 */

struct LightGroupInfo
{
    s32 copyFrom;  /* light group this one is copied from (groups 2-10) */
    u32 flags;     /* 1: reload lights, 2: reload ambient */
};

void init_light_dir(struct Light *light);
void load_light(struct LightGroup *group, int lightIdxInGroup, struct Light *light);
void init_bg_lighting(int stageId);
void init_light_groups(void);
void g_init_lights(void);
void light_init(int stageId);
void light_main(void);
void g_set_active_light_group(int lightGrpId);
int get_active_light_group(void);
void load_light_group_cached(int lightGrpId);
void push_light_group(void);
void pop_light_group(void);
void u_reset_light_group_stack(int unused);
void u_set_some_minimap_light_param(float scale);
void g_set_light_color_maybe(int lightGrpId, float r, float g, float b);
void get_curr_light_group_ambient(struct Color3f *color);
void apply_curr_light_group_ambient(void);
void u_draw_naomi_ball(void);
void u_smth_with_lights_smd_continue(int stageId);
void fn_1_7FF4(float r, float g, float b);
void fade_color_base_default(void);
void g_smth_with_avdisp_globals(float r, float g, float b);  /* set_render_ambient */

/* Elsewhere: perf timers, nl2ngc and avdisp model renderers. */
void perf_init_timer(int timer);
u32 perf_stop_timer(int timer);
void nl2ngc_set_light_mask(u32 lightMask);          /* nl2ngc: set light mask */
void fn_1_10A380(u32 lightMask);         /* avdisp: set light mask */
void fn_1_10A3A0(Vec *dir);              /* avdisp: set infinite light direction */
void fn_1_10A410(float r, float g, float b);  /* avdisp: set infinite light color */
int g_check_flag_related_to_pausing(void);
void nl2ngc_set_ambient(float r, float g, float b);              /* nl2ngc: set ambient */
void call_g_avdisp_set_ambient(float r, float g, float b);
BOOL add_light_to_pool(struct Light *light);
struct Light *alloc_pool_light(int id, int inst);
void set_bg_ambient(float r, float g, float b);

#endif
