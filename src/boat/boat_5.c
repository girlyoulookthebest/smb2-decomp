#include "boat.h"
#include "macros.h"
#include "stage.h"
#include "mathutil.h"
#include "camera.h"

extern int lbl_80198B38;
#ifdef __MWERKS__
int strlen(const char *s);
#endif
#ifdef __MWERKS__
int strncmp(const char *a, const char *b, int n);
#endif
int call_SoundReqID_arg_1_copy(u32 soundId);
void SoundVol(u16 id, u8 vol);
void SoundPan(u16 id, u8 a, u8 b);
void SoundVolID(int soundId, u8 vol);
void SoundPanID(int soundId, u8 a, u8 b);
s8 fn_1_2F5B0(int a, Vec *pos, s8 *pan, s8 *span);
s8 fn_1_2F8F0(int a, Vec *pos, s8 *pan, s8 *span);

u32 lbl_14_data_1340 = 1;

void fn_14_11524(struct StageExtraChannels *path, float t, Vec *out)
{
    if (t < 0.0f) {
        t += 100.0f;
    }
    if (t >= 100.0f) {
        t -= 100.0f;
    }
    out->x = evaluate_stagedef_keyframe(t, path->a.count, path->a.frames);
    out->y = evaluate_stagedef_keyframe(t, path->b.count, path->b.frames);
    out->z = evaluate_stagedef_keyframe(t, path->c.count, path->c.frames);
}

float fn_14_115F4(struct StageExtraChannels *path, Vec *pos, float step)
{
    float savedStep;
    Vec p;
    float dist;
    float bestT;
    float bestDist;

    savedStep = step;
    bestT = 0.0f;
    fn_14_11524(path, bestT, &p);
    bestDist = mathutil_vec_sq_distance(pos, &p);
    {
        float t;

        for (t = 0.0f; t < 100.0; t += step) {
            step = savedStep;
            fn_14_11524(path, t, &p);
            dist = mathutil_vec_sq_distance(pos, &p);
            if (dist < bestDist) {
                bestT = t;
                bestDist = dist;
            }
        }
    }
    return fn_14_11818(path, pos, bestT);
}

float fn_14_11818(struct StageExtraChannels *path, Vec *pos, float initialTime)
{
    Vec dir;
    Vec rel;
    Vec a;
    Vec b;
    float t;
    float prevDot;
    float dot;
    s16 prev;
    s16 idx;
    s16 step;
    s16 i;

    idx = __abs((s32)initialTime);
    prev = idx;
    fn_14_11524(path, prev, &a);
    fn_14_11524(path, 0.01 + prev, &b);
    PSVECSubtract(&b, &a, &dir);
    vec_normalize_len(&dir);
    PSVECSubtract(pos, &a, &rel);
    prevDot = mathutil_vec_dot_prod(&dir, &rel);
    if (prevDot >= 0.0f) {
        step = 1;
    } else {
        step = -1;
    }
    t = initialTime;
    for (i = 0; i < 100; i++) {
        idx += step;
        if (idx == 100) {
            idx = 0;
        } else if (idx == -1) {
            idx = 99;
        }
        fn_14_11524(path, idx, &a);
        fn_14_11524(path, 0.01f + (float)idx, &b);
        PSVECSubtract(&b, &a, &dir);
        vec_normalize_len(&dir);
        PSVECSubtract(pos, &a, &rel);
        dot = mathutil_vec_dot_prod(&dir, &rel);
        if (step > 0 && prevDot >= 0.0f && dot < 0.0f) {
            dot = fabs(dot);
            t = prev + prevDot / (prevDot + dot);
            break;
        }
        if (step < 0 && prevDot < 0.0f && dot >= 0.0f) {
            prevDot = fabs(prevDot);
            t = prev - prevDot / (prevDot + dot);
            break;
        }
        prev = idx;
        prevDot = dot;
    }
    if (t < 0.0) {
        t += 100.0;
    }
    if (t >= 100.0) {
        t -= 100.0;
    }
    return t;
}

float fn_14_11CE8(struct StageExtraChannels *path, Vec *pos, float step)
{
    float savedStep;
    Vec p;
    float dist;
    float bestT;
    float bestDist;

    savedStep = step;
    bestT = 0.0f;
    fn_14_11524(path, bestT, &p);
    bestDist = mathutil_vec_sq_distance(pos, &p);
    {
        float t;

        for (t = 0.0f; t < 100.0; t += step) {
            step = savedStep;
            fn_14_11524(path, t, &p);
            dist = mathutil_vec_sq_distance(pos, &p);
            if (dist < bestDist) {
                bestT = t;
                bestDist = dist;
            }
        }
    }
    return fn_14_11F0C(path, pos, bestT);
}

float fn_14_11F0C(struct StageExtraChannels *path, Vec *pos, float initialTime)
{
    Vec dir;
    Vec rel;
    Vec a;
    Vec b;
    float t;
    float prevDot;
    float dot;
    s16 prev;
    s16 idx;
    s16 step;
    s16 i;

    idx = __abs((s32)initialTime);
    prev = idx;
    fn_14_11524(path, prev, &a);
    fn_14_11524(path, 0.01 + prev, &b);
    PSVECSubtract(&b, &a, &dir);
    vec_normalize_len(&dir);
    PSVECSubtract(pos, &a, &rel);
    prevDot = mathutil_vec_dot_prod(&dir, &rel);
    if (prevDot >= 0.0f) {
        step = 1;
    } else {
        step = -1;
    }
    t = initialTime;
    for (i = 0; i < 100; i++) {
        idx += step;
        if (idx == 100) {
            t = 100.0f;
            break;
        }
        if (idx == -1) {
            t = 0.0f;
            break;
        }
        fn_14_11524(path, idx, &a);
        fn_14_11524(path, 0.01f + (float)idx, &b);
        PSVECSubtract(&b, &a, &dir);
        vec_normalize_len(&dir);
        PSVECSubtract(pos, &a, &rel);
        dot = mathutil_vec_dot_prod(&dir, &rel);
        if (step > 0 && prevDot >= 0.0f && dot < 0.0f) {
            dot = fabs(dot);
            t = prev + prevDot / (prevDot + dot);
            break;
        }
        if (step < 0 && prevDot < 0.0f && dot >= 0.0f) {
            prevDot = fabs(prevDot);
            t = prev - prevDot / (prevDot + dot);
            break;
        }
        prev = idx;
        prevDot = dot;
    }
    if (t < 0.0) {
        t = 0.0f;
    }
    if (t >= 100.0) {
        t = 100.0f;
    }
    return t;
}

void fn_14_123D0(s32 frames, float frac, struct BoatTime *out)
{
    s32 min;
    s32 secs;
    s32 ms;

    secs = frames / lbl_80198B38;
    min = secs / 60;
    secs %= 60;
    ms = (1000.0f * (frac + (float)(frames % lbl_80198B38))) / lbl_80198B34;
    secs += ms / 1000;
    ms %= 1000;
    out->min = min;
    out->sec = secs;
    out->cs = ms / 10;
}

s32 fn_14_12504(struct GMA *gma, char *name)
{
    s16 len;
    s32 count;
    s32 i;
    char *entry;

    len = strlen(name);
    count = 0;
    for (i = 0; i < (s32)gma->numModels; i++) {
        entry = gma->modelEntries[i].name;
        strlen(entry);
        if (strncmp(name, entry, len) == 0) {
            count++;
        }
    }
    return count;
}

struct GMAModel *fn_14_12598(struct GMA *gma, char *name)
{
    struct GMAModel *model;
    char *entry;
    s16 len;
    s16 i;

    model = NULL;
    len = strlen(name);
    for (i = 0; i < (s32)gma->numModels; i++) {
        entry = gma->modelEntries[i].name;
        if (strncmp(name, entry, len) == 0) {
            model = gma->modelEntries[i].model;
            break;
        }
    }
    return model;
}

void fn_14_12638(struct Camera *cam, Vec *out, float x, float y)
{
    float width;
    float height;
    float scale;
    float s;

    width = lbl_1_bss_2434 * cam->sub28.viewportW;
    height = cam->sub28.viewportH * (lbl_1_bss_2434 / g_camera_aspect_ratio);
    scale = (1.0 / (0.5 * height)) * (-out->z * cam->sub28.view.tanHalfFov);
    if (modeCtrl.wideMode != 0) {
        x = (x * lbl_1_bss_2434) / 640.0f;
        y = ((y * lbl_1_bss_2434) / g_camera_aspect_ratio) / 480.0f;
    }
    out->x = scale * (x - 0.5 * width);
    out->y = scale * -(y - 0.5 * height);
    if (modeCtrl.wideMode != 0) {
        s = lbl_1_bss_2440 / cam->sub28.view.tanHalfFov;
        out->x *= s;
        out->y *= s;
    }
}

void fn_14_1276C(struct BoatTime *a, struct BoatTime *b, struct BoatTime *out)
{
    u16 t[3] = {0, 0, 0};

    t[2] = a->cs + b->cs;
    if (t[2] >= 100) {
        t[1] = t[2] / 100;
        t[2] %= 100;
    }
    t[1] += a->sec + b->sec;
    if (t[1] >= 60) {
        t[0] = t[1] / 60;
        t[1] %= 60;
    }
    t[0] += a->min + b->min;
    if (t[0] >= 100) {
        t[0] = 99;
        t[1] = 59;
        t[2] = 99;
    }
    out->min = t[0];
    out->sec = t[1];
    out->cs = t[2];
}

u32 fn_14_128E4(u32 seed)
{
    u32 prev;

    prev = lbl_14_data_1340;
    lbl_14_data_1340 = seed;
    return prev;
}

u16 fn_14_12904(void)
{
    lbl_14_data_1340 = lbl_14_data_1340 * 0x41C64E6D + 0x3039;
    return (lbl_14_data_1340 >> 16) & 0x7FFF;
}

float fn_14_12944(void)
{
    lbl_14_data_1340 = lbl_14_data_1340 * 0x41C64E6D + 0x3039;
    return (u16)((lbl_14_data_1340 >> 16) & 0x7FFF) / 32767.0;
}

void fn_14_129C0(Vec *axis, s16 angle)
{
    Mtx m;
    float s;
    float c;
    float t;
    float x;
    float y;
    float z;
    float sx;
    float sy;
    float sz;
    float xy;
    float yz;
    float zx;
    float xx;
    float yy;
    float zz;

    s = math_sin(angle);
    c = math_sin(angle + 0x4000);
    t = 1.0f - c;
    x = axis->x;
    y = axis->y;
    z = axis->z;
    xy = t * (x * y);
    yz = t * (y * z);
    zx = t * (z * x);
    sx = x * s;
    sy = y * s;
    sz = z * s;
    xx = x * x;
    yy = y * y;
    zz = z * z;
    m[0][0] = xx + (1.0f - xx) * c;
    m[1][0] = xy + sz;
    m[2][0] = zx - sy;
    m[0][1] = xy - sz;
    m[1][1] = yy + (1.0f - yy) * c;
    m[2][1] = yz + sx;
    m[0][2] = zx + sy;
    m[1][2] = yz - sx;
    m[2][2] = zz + (1.0f - zz) * c;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    mtxa_mult_right(m);
}

void fn_14_12B24(s32 sound, void *pos)
{
    u8 pan;
    u8 span;
    s8 vol;
    int id;

    if (modeCtrl.unk30 == 1) {
        vol = fn_1_2F5B0(0, pos, (s8 *)&pan, (s8 *)&span);
    } else {
        vol = fn_1_2F8F0(modeCtrl.unk30, pos, (s8 *)&pan, (s8 *)&span);
    }
    id = call_SoundReqID_arg_1_copy((((s8)span & 0x7F) << 25) | ((((s8)pan & 0x7F) << 18) | (sound | ((vol & 0x7F) << 11))));
    SoundVol(id, vol);
    SoundPan(id, pan, span);
}

void fn_14_12BF4(s32 sound, void *pos)
{
    u8 pan;
    u8 span;
    s8 vol;

    if (modeCtrl.unk30 == 1) {
        vol = fn_1_2F5B0(0, pos, (s8 *)&pan, (s8 *)&span);
    } else {
        vol = fn_1_2F8F0(modeCtrl.unk30, pos, (s8 *)&pan, (s8 *)&span);
    }
    SoundVolID(sound, vol);
    SoundPanID(sound, pan, span);
}

