/* Lane stage: the deformable lane models and the collision surfaces for every lane type. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "gx.h"
#include "mathutil.h"
#include "stage.h"

#define LANE_HALF_WIDTH 2.4f
#define LANE_FLOOR_Y -0.39f
#define LANE_START_Z -15.86f
#define LANE_END_Z -98.01f
#define LANE_LENGTH 82.15f
#define GUTTER_EDGE 3.49f
#define PIT_BACK_Z -104.34f
#define NO_SURFACE -1000.0f

struct LaneVertex
{
    Vec pos;
    Vec normal;
    u8 unk18[0x28];
};

struct ShadowDraw
{
    sptr type;  /* ShadowReceive.obj: pointer-sized */
    s16 unk4;
    s16 unk6;
    struct GMAModel *model;
    u8 unkC[4];
};

extern s16 lbl_5_bss_1AE;
extern s16 lbl_5_bss_1B0;

static s8 currLane;
static s8 currVariant;
static Vec *basePositions;
static struct GMAModel *laneModel;
static u32 *laneVtxCount;
static struct LaneVertex *laneVtx;
static Vec *baseNormals;
static struct GMAModel *railModel;
static u32 *railVtxCount;
static struct LaneVertex *railVtx;
static Vec *railBasePositions;
static Vec *railBaseNormals;
static float (*laneHeight)(Vec *pos, Vec *normal, float radius);
static void (*laneDeform)(void);
static void (*laneBounds)(float z, float *left, float *right);
static int savedTimers[3];
int lbl_5_bss_1BB94;

u32 *fn_8006991C(struct GMAModel *model);
struct LaneVertex *fn_80069870(struct GMAModel *model);
int g_check_some_condition(int a);
void g_some_shadow_draw_func(struct ShadowDraw *a);
float math_sqrt_rsqrt(float x, float *sqrtOut);
void GXInitTexObjLOD(void *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, int biasClamp, int doEdgeLod, int maxAniso);
void fn_5_11A48(void);

void fn_5_164E4(void);
void fn_5_1661C(s8 lane, s8 variant);
void fn_5_16A30(void);
int fn_5_17380(Vec *pos, float radius);
float fn_5_17690(Vec *pos, float radius, Vec *normal);
float fn_5_1798C(Vec *pos, Vec *normal, float radius);
float fn_5_1799C(Vec *pos, Vec *normal, float radius);
float fn_5_17B50(Vec *pos, Vec *normal, float radius);
float fn_5_17D30(Vec *pos, Vec *normal, float radius);
float fn_5_17F60(Vec *pos, Vec *normal, float radius);
float fn_5_180F0(Vec *pos, Vec *normal, float radius);
float fn_5_18368(Vec *pos, Vec *normal, float radius);
float fn_5_1847C(Vec *pos, Vec *normal, float radius);
float fn_5_185F4(Vec *pos, Vec *normal, float radius);
float fn_5_18784(Vec *pos, Vec *normal, float radius);
float fn_5_188AC(Vec *pos, Vec *normal, float radius);
float fn_5_18C58(Vec *pos, Vec *normal, float radius);
float fn_5_18E0C(Vec *pos, Vec *normal, float radius);
float fn_5_18EC8(Vec *pos, Vec *normal, float radius);
float fn_5_18FDC(Vec *pos, Vec *normal, float radius);
void fn_5_19120(void);
void fn_5_192F4(void);
void fn_5_194AC(void);
void fn_5_19730(void);
void fn_5_1990C(void);
void fn_5_19BAC(void);
void fn_5_19DB0(float z, float *left, float *right);
void fn_5_19DD4(float z, float *left, float *right);
void fn_5_19F10(float z, float *left, float *right);
void fn_5_19FCC(float z, float *left, float *right);
void fn_5_1A0D0(float z, float *left, float *right);
void fn_5_1A244(float z, float *left, float *right);

void fn_5_15F2C(void)
{
    u32 i;
    u32 size;

    currLane = 0;
    currVariant = -1;
    fn_5_1661C(currLane, currVariant);
    if (basePositions != NULL)
        fn_5_164E4();
    laneModel = stage_gma->modelEntries[14].model;
    laneVtxCount = fn_8006991C(laneModel);
    laneVtx = fn_80069870(laneModel);
    size = (*laneVtxCount * sizeof(Vec) + 0x1F) & ~0x1F;
    basePositions = alloc_from_heap_or_panic(currentHeap, size, "mini_bowling_stage.c", 0x7E);
    baseNormals = alloc_from_heap_or_panic(currentHeap, size, "mini_bowling_stage.c", 0x7F);
    for (i = 0; i < *laneVtxCount; i++)
    {
        basePositions[i] = laneVtx[i].pos;
        baseNormals[i] = laneVtx[i].normal;
    }
    railModel = stage_gma->modelEntries[16].model;
    railVtxCount = fn_8006991C(railModel);
    railVtx = fn_80069870(railModel);
    size = (*railVtxCount * sizeof(Vec) + 0x1F) & ~0x1F;
    railBasePositions = alloc_from_heap_or_panic(currentHeap, size, "mini_bowling_stage.c", 0x8D);
    railBaseNormals = alloc_from_heap_or_panic(currentHeap, size, "mini_bowling_stage.c", 0x8E);
    for (i = 0; i < *railVtxCount; i++)
    {
        railBasePositions[i] = railVtx[i].pos;
        railBaseNormals[i] = railVtx[i].normal;
    }
    laneHeight = fn_5_1798C;
    laneDeform = fn_5_19120;
    laneBounds = fn_5_19DB0;
    GXInitTexObjLOD((u8 *)stage_gma->modelEntries[10].model->texObjs + 0x80, 1, 1, 0.0f, 10.0f, 0.0f, 1, 1, 1);
    GXInitTexObjLOD((u8 *)stage_gma->modelEntries[10].model->texObjs + 0xA0, 1, 1, 0.0f, 10.0f, 0.0f, 1, 1, 1);
    GXInitTexObjLOD((u8 *)laneModel->texObjs + 0x60, 1, 1, 0.0f, 10.0f, 0.0f, 1, 1, 1);
    GXInitTexObjLOD((u8 *)laneModel->texObjs + 0xC0, 1, 1, 0.0f, 10.0f, 0.0f, 1, 1, 1);
    GXInitTexObjLOD((u8 *)stage_gma->modelEntries[12].model->texObjs + 0x60, 1, 1, 0.0f, 10.0f, 0.0f, 1, 1, 1);
}

void fn_5_16358(void)
{
    struct ShadowDraw shadow;

    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    if (currLane == 0 || currVariant == -1)
    {
        avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[10].model);
        shadow.type = 10;
        shadow.model = stage_gma->modelEntries[10].model;
    }
    else
    {
        avdisp_draw_model_culled_sort_never(laneModel);
        shadow.type = 14;
        shadow.model = laneModel;
    }
    if (g_check_some_condition(4))
    {
        shadow.unk6 = 6;
        shadow.unk4 = 4;
        g_some_shadow_draw_func(&shadow);
    }
}

void fn_5_16438(void)
{
    mtxa_from_mtxb();
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[17].model);
    if (currLane == 0 || currVariant == -1)
        avdisp_draw_model_culled_sort_never(stage_gma->modelEntries[11].model);
    else
    {
        laneDeform();
        avdisp_draw_model_culled_sort_never(railModel);
    }
}

void fn_5_164E4(void)
{
    fn_5_16A30();
    if (basePositions != NULL)
    {
        OSFreeToHeap(currentHeap, basePositions);
        basePositions = NULL;
    }
    if (baseNormals != NULL)
    {
        OSFreeToHeap(currentHeap, baseNormals);
        baseNormals = NULL;
    }
    if (railBasePositions != NULL)
    {
        OSFreeToHeap(currentHeap, railBasePositions);
        railBasePositions = NULL;
    }
    if (railBaseNormals != NULL)
    {
        OSFreeToHeap(currentHeap, railBaseNormals);
        railBaseNormals = NULL;
    }
}

void fn_5_165B4(int slot)
{
    if (slot > -1 && slot < 3)
        savedTimers[slot] = lbl_5_bss_1BB94;
}

void fn_5_165E8(int slot)
{
    if (slot > -1 && slot < 3)
        lbl_5_bss_1BB94 = savedTimers[slot];
}

void fn_5_1661C(s8 lane, s8 variant)
{
    if (lane <= -1)
        lane = 0;
    if (lane >= 15)
        lane = 14;
    if (variant <= -2)
        variant = -1;
    if (variant >= 3)
        variant = 2;
    currLane = lane;
    currVariant = variant;
    if (currLane == 8)
    {
        s16 a[3] = {-0x500, -0x480, -0x400};
        s16 b[3] = {-0x200, -0x700, -0xC00};

        lbl_5_bss_1AE = (s16)a[currVariant];
        lbl_5_bss_1B0 = (s16)b[currVariant];
    }
    else
    {
        lbl_5_bss_1AE = -0x400;
        lbl_5_bss_1B0 = 0;
    }
    fn_5_16A30();
    fn_5_11A48();
    switch (currLane)
    {
    case 1:
        laneHeight = fn_5_1799C;
        break;
    case 2:
        laneHeight = fn_5_17B50;
        break;
    case 3:
        laneHeight = fn_5_17D30;
        break;
    case 4:
        laneHeight = fn_5_17F60;
        break;
    case 5:
        laneHeight = fn_5_180F0;
        break;
    case 6:
        laneHeight = fn_5_18368;
        break;
    case 7:
        laneHeight = fn_5_1847C;
        break;
    case 8:
        laneHeight = fn_5_185F4;
        break;
    case 9:
        laneHeight = fn_5_18784;
        break;
    case 10:
        laneHeight = fn_5_188AC;
        break;
    case 11:
        laneHeight = fn_5_18C58;
        break;
    case 12:
        laneHeight = fn_5_18E0C;
        break;
    case 13:
        laneHeight = fn_5_18EC8;
        break;
    case 14:
        laneHeight = fn_5_18FDC;
        break;
    case 0:
    default:
        laneHeight = fn_5_1798C;
        break;
    }
    switch (currLane)
    {
    case 7:
        laneDeform = fn_5_192F4;
        break;
    case 8:
        laneDeform = fn_5_194AC;
        break;
    case 9:
        laneDeform = fn_5_19730;
        break;
    case 10:
        laneDeform = fn_5_1990C;
        break;
    case 14:
        laneDeform = fn_5_19BAC;
        break;
    default:
        laneDeform = fn_5_19120;
        break;
    }
    switch (currLane)
    {
    case 7:
        laneBounds = fn_5_19DD4;
        break;
    case 8:
        laneBounds = fn_5_19F10;
        break;
    case 9:
        laneBounds = fn_5_19FCC;
        break;
    case 10:
        laneBounds = fn_5_1A0D0;
        break;
    case 14:
        laneBounds = fn_5_1A244;
        break;
    default:
        laneBounds = fn_5_19DB0;
        break;
    }
    if (currVariant == -1)
    {
        laneHeight = fn_5_1798C;
        laneDeform = fn_5_19120;
        laneBounds = fn_5_19DB0;
    }
}

void fn_5_169B4(s8 lane)
{
    fn_5_1661C(lane, currVariant);
}

s8 fn_5_169E0(void)
{
    return currLane;
}

void fn_5_169F0(s8 variant)
{
    fn_5_1661C(currLane, variant);
}

s8 fn_5_16A20(void)
{
    return currVariant;
}

void fn_5_16A30(void)
{
    u32 i;

    if (basePositions != NULL && baseNormals != NULL)
    {
        for (i = 0; i < *laneVtxCount; i++)
        {
            laneVtx[i].pos = basePositions[i];
            laneVtx[i].normal = baseNormals[i];
        }
    }
    if (railBasePositions != NULL && railBaseNormals != NULL)
    {
        for (i = 0; i < *railVtxCount; i++)
        {
            railVtx[i].pos = railBasePositions[i];
            railVtx[i].normal = railBaseNormals[i];
        }
    }
}

float fn_5_16B64(Vec *pos)
{
    Vec normal;
    Vec gutterNormal;
    float left;
    float right;

    if (pos->y < -5.0f)
        return NO_SURFACE;
    if (pos->z < PIT_BACK_Z)
        return NO_SURFACE;
    if (pos->z > 5.1f)
        return NO_SURFACE;
    if (pos->z > LANE_START_Z)
    {
        if (pos->x < -LANE_HALF_WIDTH)
            return NO_SURFACE;
        return pos->x > LANE_HALF_WIDTH ? NO_SURFACE : LANE_FLOOR_Y;
    }
    if (pos->z > LANE_END_Z)
    {
        laneBounds(pos->z, &left, &right);
        if (pos->x < left)
            return NO_SURFACE;
        if (pos->x > right)
            return NO_SURFACE;
        return pos->z > LANE_START_Z ? LANE_FLOOR_Y : laneHeight(pos, &normal, 0.0f);
    }
    if (pos->x < -GUTTER_EDGE)
        return NO_SURFACE;
    if (pos->x > GUTTER_EDGE)
        return NO_SURFACE;
    if (pos->x < -LANE_HALF_WIDTH || pos->x > LANE_HALF_WIDTH)
        return fn_5_17690(pos, 0.0f, &gutterNormal);
    return NO_SURFACE;
}

float fn_5_16D30(Vec *pos, Vec *prevPos, float radius, Vec *normal, float *b, int *c)
{
    float result = 1.0f;
    float depth;
    int hit = fn_5_17380(pos, radius);
    int prevZone = fn_5_17380(prevPos, radius);
    int zone;

    if (hit == prevZone)
        zone = hit;
    else if (prevZone == 16 && (hit == 2 || hit == 6 || hit == 0 || hit == 1))
        zone = 11;
    else if (prevZone == 6 && (hit == 2 || hit == 0 || hit == 1))
        zone = 11;
    else if (prevZone == 15)
    {
        switch (hit)
        {
        case 0:
        case 1:
        case 2:
        case 9:
        case 10:
            zone = 14;
            break;
        case 4:
        case 5:
            zone = hit;
            break;
        case 3:
        default:
            zone = 15;
            break;
        }
    }
    else if (prevZone == 17)
    {
        if (hit == 0)
            zone = 14;
        else if (hit == 9)
            zone = 14;
        else if (hit == 4)
            zone = 4;
        else if (hit == 2)
            zone = 12;
        else
            zone = 17;
    }
    else if (prevZone == 18)
    {
        if (hit == 1)
            zone = 14;
        else if (hit == 10)
            zone = 14;
        else if (hit == 5)
            zone = 5;
        else if (hit == 2)
            zone = 13;
        else
            zone = 18;
    }
    else if (prevZone == 2)
    {
        if (hit == 17)
            zone = 12;
        else if (hit == 18)
            zone = 13;
        else
            zone = hit;
    }
    else
        zone = hit;

    switch (zone)
    {
    case 0:
    case 1:
        depth = fn_5_17690(pos, radius, normal);
        result = 0.5f;
        break;
    case 2:
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
        if (currLane == 0 || currVariant == -1 || pos->z >= LANE_START_Z || pos->z <= LANE_END_Z)
            depth = (LANE_FLOOR_Y - pos->y) + radius;
        else
            depth = radius + (laneHeight(pos, normal, radius) - pos->y);
        if (pos->z > -66.67f && pos->x > -2.1f && pos->x < 2.1f)
            zone = 3;
        break;
    case 4:
    {
        Vec edge;
        float left;
        float right;
        float y;

        laneBounds(pos->z, &left, &right);
        edge.x = left;
        edge.y = pos->y;
        edge.z = pos->z;
        y = laneHeight(&edge, normal, radius);
        normal->x = pos->x - left;
        normal->y = pos->y - y;
        normal->z = 0.0f;
        depth = radius - vec_normalize_len(normal);
        break;
    }
    case 5:
    {
        Vec edge;
        float left;
        float right;
        float y;

        laneBounds(pos->z, &left, &right);
        edge.x = right;
        edge.y = pos->y;
        edge.z = pos->z;
        y = laneHeight(&edge, normal, radius);
        normal->x = pos->x - right;
        normal->y = pos->y - y;
        normal->z = 0.0f;
        depth = radius - vec_normalize_len(normal);
        break;
    }
    case 6:
        normal->x = 0.0f;
        normal->y = pos->y - LANE_FLOOR_Y;
        normal->z = pos->z - PIT_BACK_Z;
        depth = radius - vec_normalize_len(normal);
        break;
    case 7:
        normal->x = 0.0f;
        normal->y = -1.0f;
        normal->z = 0.0f;
        depth = -1.673f + pos->y + radius;
        break;
    case 8:
        normal->x = 0.0f;
        normal->y = 0.0f;
        normal->z = 0.5f;
        depth = (-106.629f - pos->z) + radius;
        result = 0.3f;
        break;
    case 9:
        normal->x = 1.0f;
        normal->y = 0.0f;
        normal->z = 0.0f;
        depth = (-GUTTER_EDGE - pos->x) + radius;
        result = 0.5f;
        break;
    case 10:
        normal->x = -1.0f;
        normal->y = 0.0f;
        normal->z = 0.0f;
        depth = -GUTTER_EDGE + pos->x + radius;
        result = 0.5f;
        break;
    case 11:
        normal->x = 0.0f;
        normal->y = 0.0f;
        normal->z = -1.0f;
        depth = 104.34f + pos->z + radius;
        break;
    case 12:
    {
        float left;
        float right;

        laneBounds(pos->z, &left, &right);
        normal->x = -0.99f;
        normal->y = 0.0f;
        normal->z = 0.1f;
        vec_normalize_len(normal);
        depth = radius + (-left + pos->x);
        break;
    }
    case 13:
    {
        float left;
        float right;

        laneBounds(pos->z, &left, &right);
        normal->x = 0.99f;
        normal->y = 0.0f;
        normal->z = 0.1f;
        vec_normalize_len(normal);
        depth = radius + (right - pos->x);
        break;
    }
    case 14:
        normal->x = 0.0f;
        normal->y = 0.0f;
        normal->z = 1.0f;
        depth = (LANE_END_Z - pos->z) + radius;
        break;
    case 3:
    case 15:
    case 16:
    case 17:
    case 18:
    default:
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
        depth = NO_SURFACE;
        break;
    }
    *c = zone;
    *b = result;
    return depth;
}

void fn_5_17350(float z, float *left, float *right)
{
    laneBounds(z, left, right);
}

int fn_5_17380(Vec *pos, float radius)
{
    Vec normal;
    float left;
    float right;
    float y;

    if (pos->y < -5.0f)
        return 15;
    if (pos->y - radius > 3.5f)
        return 15;
    if (pos->z > 5.1f)
        return 15;
    if (pos->z > LANE_END_Z)
    {
        if (pos->z > LANE_START_Z)
        {
            left = -LANE_HALF_WIDTH;
            right = LANE_HALF_WIDTH;
        }
        else
            laneBounds(pos->z, &left, &right);
        if (pos->x < left)
        {
            if (pos->x > left - radius)
            {
                Vec edge;

                edge.x = left;
                edge.y = pos->y;
                edge.z = pos->z;
                if (pos->y >= laneHeight(&edge, &normal, radius))
                    return 4;
            }
            return 17;
        }
        if (pos->x > right)
        {
            if (pos->x < right + radius)
            {
                Vec edge;

                edge.x = right;
                edge.y = pos->y;
                edge.z = pos->z;
                if (pos->y >= laneHeight(&edge, &normal, radius))
                    return 5;
            }
            return 18;
        }
        if (pos->z > LANE_START_Z)
            y = LANE_FLOOR_Y;
        else
            y = laneHeight(pos, &normal, radius);
        if (pos->y < y - radius)
            return 15;
        return 2;
    }
    if (pos->y + radius > 1.673f)
        return 7;
    if (pos->z < -106.629f)
        return 8;
    if (pos->x - radius < -GUTTER_EDGE)
        return 9;
    if (pos->x + radius > GUTTER_EDGE)
        return 10;
    if (pos->z + radius < PIT_BACK_Z)
        return 16;
    if (pos->z < PIT_BACK_Z)
        return 6;
    if (pos->x < -LANE_HALF_WIDTH)
        return 0;
    if (pos->x > LANE_HALF_WIDTH)
        return 1;
    return 2;
}

float fn_5_17690(Vec *pos, float radius, Vec *normal)
{
    Vec v;
    float outDist;
    float inDist;
    float dx;
    float dy;
    float arcRadius;
    float dx2;
    float dy2;
    float result;
    float edgeIn;
    float rimY;
    float center;
    float edgeOut;
    float centerY;
    float radiusSq;
    float sqA;
    float t;
    float sqB;

    result = (-0.391 - pos->y) + radius;
    rimY = -0.391f;
    radiusSq = radius * radius;
    if (result < 0.0f)
    {
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
        return result;
    }
    if (pos->x < 0.0f)
    {
        edgeIn = -3.49f;
        edgeOut = -2.39f;
        center = -2.94f;
    }
    else
    {
        edgeIn = 2.39f;
        edgeOut = 3.49f;
        center = 2.94f;
    }
    t = 0.15784073869465684 * (-98.004499999999993 - pos->z);
    arcRadius = 0.51518301600000005 + (0.049426822999999898 * t);
    centerY = -0.28481698300000002 + (0.021426822000000012 * t);
    dx = pos->x - edgeIn;
    dy = pos->y - rimY;
    if (0.0 == dx && 0.0 == dy)
        dx = 1.0f;
    sqA = dx * dx + dy * dy;
    dx2 = pos->x - edgeOut;
    dy2 = pos->y - rimY;
    if (0.0 == dx2 && 0.0 == dy2)
        dx2 = -1.0f;
    sqB = dx2 * dx2 + dy2 * dy2;
    if (radiusSq > sqA)
    {
        float invLen = math_sqrt_rsqrt(sqA, &outDist);

        v.x = dx * invLen;
        v.y = dy * invLen;
        v.z = 0.0f;
        result = radius - outDist;
    }
    else if (radiusSq > sqB)
    {
        float invLen = math_sqrt_rsqrt(sqB, &inDist);

        v.x = dx2 * invLen;
        v.y = dy2 * invLen;
        v.z = 0.0f;
        result = radius - inDist;
    }
    else
    {
        float len;

        v.x = 10000.0 * (center - pos->x);
        v.y = 10000.0 * (centerY - pos->y);
        v.z = 0.0f;
        if (0.0 == v.x && 0.0 == v.y)
        {
            v.y = 1.0f;
            len = 0.0f;
        }
        else
            len = 0.0001 * vec_normalize_len(&v);
        result = radius + (len - arcRadius);
    }
    v.y += 1.0;
    v.z -= 0.05;
    vec_normalize_len(&v);
    *normal = v;
    return result;
}

float fn_5_1798C(Vec *pos, Vec *normal, float radius)
{
    return LANE_FLOOR_Y;
}

float fn_5_1799C(Vec *pos, Vec *normal, float radius)
{
    s16 speeds[3] = {0x600, 0x800, 0xC00};
    float amps[3] = {0.6f, 0.9f, 1.4f};
    s16 along = (65536.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH;
    s16 wave = 2.0 * along - (u16)lbl_5_bss_1BB94 * speeds[currVariant];
    float amp = amps[currVariant] * (0.5 + 0.5 * math_sin(along + 0xC000));

    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.15 * amp * math_sin(wave + 0x4000);
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + amp * math_sin(wave);
}

float fn_5_17B50(Vec *pos, Vec *normal, float radius)
{
    Vec bumps[3][3] = {
        {{-1.8f, 1.0f, -75.0f}, {1.8f, 0.0f, -55.0f}, {0.0f, 0.0f, -35.0f}},
        {{-1.8f, 1.2f, -75.0f}, {1.8f, 1.0f, -55.0f}, {0.0f, 0.0f, -35.0f}},
        {{-1.8f, 1.5f, -75.0f}, {1.8f, 1.2f, -55.0f}, {-1.8f, 1.0f, -35.0f}},
    };
    int i;

    for (i = 0; i < 3; i++)
    {
        Vec *bump = &bumps[currVariant][i];
        float distSq = mathutil_sum_of_sq_2(pos->x - bump->x, pos->z - bump->z);

        if (distSq < 17.639997f)
        {
            float dist = math_sqrt(distSq);
            s16 angle = 32768.0f + 32768.0f * (dist / 4.2f);

            normal->x = 0.1 * bump->y * math_sin(angle);
            normal->y = 1.0f;
            normal->z = -0.1 * bump->y * math_sin(angle);
            if (pos->x - bump->x > 0.0f)
                normal->x = -normal->x;
            if (pos->z - bump->z < 0.0f)
                normal->z = -normal->z;
            vec_normalize_len(normal);
            return LANE_FLOOR_Y - (0.5 * bump->y * (math_sin(angle + 0x4000) - 1.0f));
        }
    }
    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    return LANE_FLOOR_Y;
}

float fn_5_17D30(Vec *pos, Vec *normal, float radius)
{
    s16 angle = 32768.0f + (32768.0f * (-LANE_HALF_WIDTH - pos->x)) / -4.8f;
    float fade;
    float amp;
    float slope;

    if (currVariant == 0)
        amp = 1.0f;
    else if (currVariant == 1)
        amp = fabs(math_sin(((u8)lbl_5_bss_1BB94 << 8) + 0x4000));
    else
        amp = 0.2 + 0.8 * math_sin(((u8)lbl_5_bss_1BB94 << 8) + 0x4000);
    if (pos->z > LANE_START_Z - 5.0)
    {
        fade = 0.2 * (pos->z - LANE_START_Z);
        slope = -1.0 - fade;
    }
    else if (pos->z < LANE_END_Z + 5.0)
    {
        fade = 0.2 * (pos->z - LANE_END_Z);
        slope = 1.0 - fade;
    }
    else
    {
        fade = 1.0f;
        slope = 0.0f;
    }
    normal->x = -0.42 * amp * fabs(fade) * math_sin(angle + 0x4000);
    normal->y = 1.0f;
    normal->z = 0.3 * amp * slope;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + 1.2 * amp * fabs(fade) * math_sin(angle);
}

float fn_5_17F60(Vec *pos, Vec *normal, float radius)
{
    int freqs[3] = {0x8000, 0x10000, 0x18000};
    int phases[3] = {0, 0x8000, 0};
    float slopes[3] = {0.05f, 0.1f, 0.15f};
    s16 angle = ((LANE_START_Z - pos->z) * freqs[currVariant]) / LANE_LENGTH + phases[currVariant];

    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 1.3 * slopes[currVariant] * math_sin(angle + 0x4000);
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + 1.3 * math_sin(angle);
}

float fn_5_180F0(Vec *pos, Vec *normal, float radius)
{
    float sways[3] = {16384.0f, 22528.0f, 28672.0f};
    float amps[3] = {0.4f, 0.6f, 0.8f};
    s16 sway = 16384.0f + sways[currVariant] * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    s16 angle = (65536.0f * (-LANE_HALF_WIDTH - pos->x)) / -4.8f + sway;
    float fade;
    float slope;

    if (-LANE_HALF_WIDTH > pos->x || LANE_HALF_WIDTH < pos->x)
        return NO_SURFACE;
    if (pos->z > LANE_START_Z - 5.0)
    {
        fade = 0.2 * (pos->z - LANE_START_Z);
        slope = -1.0 - fade;
    }
    else if (pos->z < LANE_END_Z + 5.0)
    {
        fade = 0.2 * (pos->z - LANE_END_Z);
        slope = 1.0 - fade;
    }
    else
    {
        fade = 1.0f;
        slope = 0.0f;
    }
    normal->x = -0.35 * amps[currVariant] * fabs(fade) * math_sin(angle + 0x4000);
    normal->y = 1.0f;
    normal->z = 0.3 * slope;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + amps[currVariant] * fabs(fade) * math_sin(angle);
}

float fn_5_18368(Vec *pos, Vec *normal, float radius)
{
    float amps[3] = {0.8f, 1.0f, 1.2f};
    float slope = amps[currVariant] / -4.8f;
    float across = pos->x * slope;
    float wave = math_sin((65536.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH);

    normal->x = -0.4 * wave * slope;
    normal->y = 1.0f;
    normal->z = 0.0f;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + wave * across;
}

float fn_5_1847C(Vec *pos, Vec *normal, float radius)
{
    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    if (pos->z < -66.0f && pos->z > -90.0f)
    {
        float scales[3] = {1.0f, 0.75f, 0.5f};
        float sways[3] = {2.4f, 2.1f, 1.8f};
        float offset = sways[currVariant] * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);

        if (offset + -LANE_HALF_WIDTH * scales[currVariant] > pos->x
         || offset + LANE_HALF_WIDTH * scales[currVariant] < pos->x)
            return NO_SURFACE;
    }
    else if (-LANE_HALF_WIDTH > pos->x || LANE_HALF_WIDTH < pos->x)
        return NO_SURFACE;
    return LANE_FLOOR_Y;
}

float fn_5_185F4(Vec *pos, Vec *normal, float radius)
{
    float sways[3] = {4.8f, 8.9f, 13.0f};
    float slopes[3] = {0.1f, 0.2f, 0.3f};
    float wave = math_sin((32768.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH);
    float offset = wave * sways[currVariant];
    float slope = wave * slopes[currVariant];
    float across = (-LANE_HALF_WIDTH - pos->x) / -4.8f;

    if (pos->x < -LANE_HALF_WIDTH + offset || pos->x > LANE_HALF_WIDTH + offset)
    {
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
        return NO_SURFACE;
    }
    normal->x = -0.4 * slope;
    normal->y = 1.0f;
    normal->z = 0.0f;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + slope * across;
}

float fn_5_18784(Vec *pos, Vec *normal, float radius)
{
    float sways[3] = {2.6f, 3.9f, 5.2f};
    s16 angle = (lbl_5_bss_1BB94 & 0x1FF) << 7;
    float wave = math_sin(angle);
    float wave2 = wave * math_sin((32768.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH);
    float offset = wave2 * sways[currVariant];

    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    if (pos->x < -LANE_HALF_WIDTH + offset || pos->x > LANE_HALF_WIDTH + offset)
        return NO_SURFACE;
    return LANE_FLOOR_Y;
}

float fn_5_188AC(Vec *pos, Vec *normal, float radius)
{
    float amps[3] = {0.77f, 0.88f, 0.99f};
    s16 speeds[3] = {0x800, 0x400, 0x200};
    s8 freqs[3] = {3, 4, 5};
    s16 phase = (lbl_5_bss_1BB94 & 0x1FF) * speeds[currVariant];
    int along = (65536.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH;
    float env = 0.5 - 0.5 * math_sin(along + 0x4000);
    float width = 1.0 + env * amps[currVariant] * math_sin(freqs[currVariant] * along - phase + 0x4000);
    float left = -LANE_HALF_WIDTH * width;
    float right = LANE_HALF_WIDTH * width;

    if (left - radius > pos->x || right + radius < pos->x || LANE_FLOOR_Y - radius > pos->y)
    {
        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.0f;
        return NO_SURFACE;
    }
    if (left > pos->x)
    {
        float wave = env * math_sin(freqs[currVariant] * along - phase);

        normal->x = (pos->x - left) * (1.0 - 0.5 * __abs((int)wave));
        normal->y = pos->y - LANE_FLOOR_Y;
        normal->z = -0.1 * wave;
        return (LANE_FLOOR_Y + radius) - vec_normalize_len(normal);
    }
    if (right < pos->x)
    {
        float wave = env * math_sin(freqs[currVariant] * along - phase);

        normal->x = (pos->x - right) * (1.0 - 0.5 * __abs((int)wave));
        normal->y = pos->y - LANE_FLOOR_Y;
        normal->z = -0.1 * wave;
        return (LANE_FLOOR_Y + radius) - vec_normalize_len(normal);
    }
    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    return LANE_FLOOR_Y;
}

float fn_5_18C58(Vec *pos, Vec *normal, float radius)
{
    s16 angle = (65536.0f * (-LANE_HALF_WIDTH - pos->x)) / -4.8f;
    float s2 = math_sin((angle * 2) + 0x4000);
    float s1 = math_sin(angle + 0x4000) ;
    float height;
    float fade;
    float sum;

    s1 = s1 + s2;
    height = 0.25 * s1;
    if (pos->z < LANE_START_Z && pos->z > LANE_START_Z - 5.0)
        fade = 0.2 * (LANE_START_Z - pos->z);
    else if (pos->z > LANE_END_Z && pos->z < LANE_END_Z + 5.0)
        fade = 0.2 * (pos->z - LANE_END_Z);
    else
        fade = 1.0f;
    s2 = math_sin(angle * 2);
    s1 = math_sin(angle);
    sum = s1 + s2;
    normal->x = 0.25 * (0.8 * fade) * sum;
    normal->y = 1.0f;
    normal->z = 0.0f;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + fade * height;
}

float fn_5_18E0C(Vec *pos, Vec *normal, float radius)
{
    if (pos->z > -74.015501785278317)
    {
        float slope = 0.024345709f;

        normal->x = 0.0f;
        normal->y = 1.0f;
        normal->z = 0.05 * slope;
        vec_normalize_len(normal);
        return LANE_FLOOR_Y + slope * (LANE_START_Z - pos->z);
    }
    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    return LANE_FLOOR_Y;
}

float fn_5_18EC8(Vec *pos, Vec *normal, float radius)
{
    float slope;
    float height;
    float split = -91.096f;

    if (pos->z > split)
    {
        slope = -0.015824711f;
        height = slope * (LANE_START_Z - pos->z);
        slope *= 0.2;
    }
    else
    {
        s16 angle = 49152.0f + (16384.0f * (split - pos->z)) / (split - LANE_END_Z);

        slope = 0.325 * math_sin(angle + 0x4000);
        height = 1.3 * math_sin(angle);
    }
    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = slope;
    vec_normalize_len(normal);
    return LANE_FLOOR_Y + height;
}

float fn_5_18FDC(Vec *pos, Vec *normal, float radius)
{
    float wave = math_sin((int)((65536.0f * (LANE_START_Z - pos->z)) / LANE_LENGTH) + 0x4000);
    float width = 0.65 + 0.35 * wave;
    float sway = 2.0400000810623169 * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    float offset = sway * (0.5 - 0.5 * wave);

    normal->x = 0.0f;
    normal->y = 1.0f;
    normal->z = 0.0f;
    if (pos->x < -LANE_HALF_WIDTH * width + offset || pos->x > LANE_HALF_WIDTH * width + offset)
        return NO_SURFACE;
    return LANE_FLOOR_Y;
}

void fn_5_19120(void)
{
    Vec normal;
    Vec lanePos;
    Vec railPos;
    u32 i;

    for (i = 0; i < *laneVtxCount; i++)
    {
        lanePos = basePositions[i];
        if (lanePos.z >= LANE_START_Z || lanePos.z <= LANE_END_Z)
            continue;
        laneVtx[i].pos.y = lanePos.y + (laneHeight(&lanePos, &normal, 0.0f) - LANE_FLOOR_Y);
        if ((int)i >= 0x2D0)
            laneVtx[i].normal = normal;
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        railPos = railBasePositions[i];
        if (railPos.z >= LANE_START_Z || railPos.z <= LANE_END_Z)
            continue;
        railVtx[i].pos.y = railPos.y + (laneHeight(&railPos, &normal, 0.0f) - LANE_FLOOR_Y);
        railVtx[i].normal = normal;
    }
}

void fn_5_192F4(void)
{
    float scales[3] = {1.0f, 0.75f, 0.5f};
    float sways[3] = {2.4f, 2.1f, 1.8f};
    float offset = sways[currVariant] * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    u32 i;

    for (i = 0; i < *laneVtxCount; i++)
    {
        if (basePositions[i].z >= -66.0f || basePositions[i].z <= -90.0f)
            continue;
        laneVtx[i].pos.x = offset + basePositions[i].x * scales[currVariant];
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        if (railBasePositions[i].z >= -66.0f || railBasePositions[i].z <= -90.0f)
            continue;
        railVtx[i].pos.x = offset + railBasePositions[i].x * scales[currVariant];
    }
}

void fn_5_194AC(void)
{
    float sways[3] = {4.8f, 8.9f, 13.0f};
    Vec normal;
    Vec lanePos;
    Vec railPos;
    u32 i;

    for (i = 0; i < *laneVtxCount; i++)
    {
        float dy;

        lanePos = basePositions[i];
        if (lanePos.z <= LANE_END_Z)
            continue;
        lanePos.x += sways[currVariant] * math_sin((32768.0f * (LANE_START_Z - lanePos.z)) / LANE_LENGTH);
        dy = laneHeight(&lanePos, &normal, 0.0f) - LANE_FLOOR_Y;
        laneVtx[i].pos.x = lanePos.x;
        laneVtx[i].pos.y = lanePos.y + dy;
        if ((int)i >= 0x2D0)
            laneVtx[i].normal = normal;
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        float dy;

        railPos = railBasePositions[i];
        if (railPos.z <= LANE_END_Z)
            continue;
        railPos.x += sways[currVariant] * math_sin((32768.0f * (LANE_START_Z - railPos.z)) / LANE_LENGTH);
        dy = laneHeight(&railPos, &normal, 0.0f) - LANE_FLOOR_Y;
        railVtx[i].pos.x = railPos.x;
        railVtx[i].pos.y = railPos.y + dy;
        railVtx[i].normal = normal;
    }
}

void fn_5_19730(void)
{
    float sways[3] = {2.6f, 3.9f, 5.2f};
    float wave = math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    u32 i;

    for (i = 0; i < *laneVtxCount; i++)
    {
        float wave2 = wave * math_sin((32768.0f * (LANE_START_Z - basePositions[i].z)) / LANE_LENGTH);
        float offset = wave2 * sways[currVariant];

        if (laneVtx[i].pos.z <= LANE_END_Z)
            continue;
        laneVtx[i].pos.x = offset + basePositions[i].x;
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        float wave2 = wave * math_sin((32768.0f * (LANE_START_Z - railBasePositions[i].z)) / LANE_LENGTH);
        float offset = wave2 * sways[currVariant];

        if (railVtx[i].pos.z <= LANE_END_Z)
            continue;
        railVtx[i].pos.x = offset + railBasePositions[i].x;
    }
}

void fn_5_1990C(void)
{
    float amps[3] = {0.77f, 0.88f, 0.99f};
    s16 speeds[3] = {0x800, 0x400, 0x200};
    s8 freqs[3] = {3, 4, 5};
    u32 i;
    s16 phase = (lbl_5_bss_1BB94 & 0x1FF) * speeds[currVariant];

    for (i = 0; i < *laneVtxCount; i++)
    {
        int along = (65536.0f * (LANE_START_Z - basePositions[i].z)) / LANE_LENGTH;
        float env = 0.5 - 0.5 * math_sin(along + 0x4000);
        float width = 1.0 + env * amps[currVariant] * math_sin(freqs[currVariant] * along - phase + 0x4000);

        if (laneVtx[i].pos.z <= LANE_END_Z)
            continue;
        laneVtx[i].pos.x = width * basePositions[i].x;
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        int along = (65536.0f * (LANE_START_Z - railBasePositions[i].z)) / LANE_LENGTH;
        float env = 0.5 - 0.5 * math_sin(along + 0x4000);
        float width = 1.0 + env * amps[currVariant] * math_sin(freqs[currVariant] * along - phase + 0x4000);

        if (railVtx[i].pos.z <= LANE_END_Z)
            continue;
        railVtx[i].pos.x = width * railBasePositions[i].x;
    }
}

void fn_5_19BAC(void)
{
    float sway = 2.0400000810623169 * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    u32 i;

    for (i = 0; i < *laneVtxCount; i++)
    {
        float wave = math_sin((int)((65536.0f * (LANE_START_Z - basePositions[i].z)) / LANE_LENGTH) + 0x4000);
        float width = 0.65 + 0.35 * wave;
        float offset = sway * (0.5 - 0.5 * wave);

        if (laneVtx[i].pos.z <= LANE_END_Z)
            continue;
        laneVtx[i].pos.x = offset + width * basePositions[i].x;
    }
    for (i = 0; i < *railVtxCount; i++)
    {
        float wave = math_sin((int)((65536.0f * (LANE_START_Z - railBasePositions[i].z)) / LANE_LENGTH) + 0x4000);
        float width = 0.65 + 0.35 * wave;
        float offset = sway * (0.5 - 0.5 * wave);

        if (laneVtx[i].pos.z <= LANE_END_Z)
            continue;
        railVtx[i].pos.x = offset + width * railBasePositions[i].x;
    }
}

void fn_5_19DB0(float z, float *left, float *right)
{
    *left = -2.4f;
    *right = 2.4f;
}

void fn_5_19DD4(float z, float *left, float *right)
{
    if (z >= -66.0f || z <= -90.0f)
    {
        *left = -LANE_HALF_WIDTH;
        *right = LANE_HALF_WIDTH;
    }
    else
    {
        float scales[3] = {1.0f, 0.75f, 0.5f};
        float sways[3] = {2.4f, 2.1f, 1.8f};
        float offset = sways[currVariant] * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);

        *left = offset + -LANE_HALF_WIDTH * scales[currVariant];
        *right = offset + LANE_HALF_WIDTH * scales[currVariant];
    }
}

void fn_5_19F10(float z, float *left, float *right)
{
    float sways[3] = {4.8f, 8.9f, 13.0f};
    float wave = math_sin((32768.0f * (LANE_START_Z - z)) / LANE_LENGTH);
    float offset = wave * sways[currVariant];

    *left = -LANE_HALF_WIDTH + offset;
    *right = LANE_HALF_WIDTH + offset;
}

void fn_5_19FCC(float z, float *left, float *right)
{
    float sways[3] = {2.6f, 3.9f, 5.2f};
    s16 angle = (lbl_5_bss_1BB94 & 0x1FF) << 7;
    float wave = math_sin(angle);
    float wave2 = wave * math_sin((32768.0f * (LANE_START_Z - z)) / LANE_LENGTH);
    float offset = wave2 * sways[currVariant];

    *left = -LANE_HALF_WIDTH + offset;
    *right = LANE_HALF_WIDTH + offset;
}

void fn_5_1A0D0(float z, float *left, float *right)
{
    float amps[3] = {0.77f, 0.88f, 0.99f};
    s16 speeds[3] = {0x800, 0x400, 0x200};
    s8 freqs[3] = {3, 4, 5};
    s16 phase = (lbl_5_bss_1BB94 & 0x1FF) * speeds[currVariant];
    int along = (65536.0f * (LANE_START_Z - z)) / LANE_LENGTH;
    float env = 0.5 - 0.5 * math_sin(along + 0x4000);
    float width = 1.0 + env * amps[currVariant] * math_sin(freqs[currVariant] * along - phase + 0x4000);

    *left = -LANE_HALF_WIDTH * width;
    *right = LANE_HALF_WIDTH * width;
}

void fn_5_1A244(float z, float *left, float *right)
{
    float sway = 2.0400000810623169 * math_sin((lbl_5_bss_1BB94 & 0x1FF) << 7);
    float wave = math_sin((int)((65536.0f * (LANE_START_Z - z)) / LANE_LENGTH) + 0x4000);
    float width = 0.65 + 0.35 * wave;
    float offset = sway * (0.5 - 0.5 * wave);

    *left = -LANE_HALF_WIDTH * width + offset;
    *right = LANE_HALF_WIDTH * width + offset;
}
