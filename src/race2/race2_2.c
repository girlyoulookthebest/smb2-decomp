#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "shadow.h"
#include "event.h"
#include "input.h"

struct ShadowTexInfo
{
    /*0x00*/ u8 unk0;    /* 0: perspective, otherwise orthographic (shadow_tex.c) */
    /*0x01*/ u8 ramp;
    /*0x02*/ u16 flags;  /* bytes: as one u32 the initializer was byte-swapped on little endian */
    /*0x04*/ GXTexObj texObj;
};  /* size = 0x24 */

struct ShadowCastReq
{
    /*0x00*/ void *obj;
    /*0x04*/ Vec lightPos;
    /*0x10*/ float unk10;
    /*0x14*/ Vec pos;
    /*0x20*/ float radius;
    /*0x24*/ Vec up;
    /*0x30*/ s16 unk30;
    /*0x32*/ u16 cameraMask;
    /*0x34*/ float unk34;
    /*0x38*/ float unk38;
    /*0x3C*/ GXColor color;
    /*0x40*/ struct ShadowTexInfo tex;
};  /* size = 0x64 */

struct Race2Marker
{
    /*0x00*/ Vec pos;
    /*0x0C*/ s16 rotX;
    /*0x0E*/ s16 rotY;
    /*0x10*/ s16 rotZ;
};  /* size = 0x14 */

extern struct Light s_lightPool[];
extern float lbl_1_data_2E418[][3];
extern u32 lbl_1_bss_D3360;
extern u16 lbl_1_bss_D3364;
extern s16 lbl_1_bss_D3368;
extern float lbl_1_bss_D3388[][4];
extern u32 lbl_1_bss_D4188;
extern u32 lbl_1_bss_D418C[13];
extern s16 player_count;

void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void fn_1_D6F9C(int);
void fn_1_D6D84(int);
void g_draw_stage(void);
void g_draw_bg(void);
void draw_shadow_reqs(void);
void draw_items(void);
void g_draw_stobjs(void);
void g_draw_effects(void);
void g_draw_ball_and_ape(void);
void handle_test_camera(void);
void ord_tbl_draw_nodes(void);
void g_something_with_drawing_apes(void);
void g_something_with_rend_efc(int);
void fn_1_120AD8(void);
int fn_1_3F61C(char *name);
void fn_1_E3164(int n, Vec *points, GXColor *color);
void fn_1_E3014(Vec *a, Vec *b, GXColor *color);
void fn_1_D4650(struct ShadowCastReq *req);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void camera_setup_default_projection(void);
void camera_apply_viewport_if_changed(int);
void mtxa_from_mtxb_translate_xyz(float x, float y, float z);
void mtxa_to_mtx(Mtx m);
#ifdef __MWERKS__
int strncmp(const char *a, const char *b, unsigned long n);
#endif
#ifdef __MWERKS__
unsigned long strlen(const char *s);
#endif
int GXGetTexObjMipMap(GXTexObj *obj);
int GXGetTexObjFmt(GXTexObj *obj);
int GXGetTexObjHeight(GXTexObj *obj);
int GXGetTexObjWidth(GXTexObj *obj);
void *GXGetTexObjData(GXTexObj *obj);
void GXInitTexObj(GXTexObj *obj, void *image, int w, int h, int fmt, int ws, int wt, int mipmap);
void GXInitTexObjLOD(GXTexObj *obj, int minFilt, int magFilt, float minLod, float maxLod, float lodBias, int biasClamp, int doEdgeLod, int maxAniso);
#ifdef __MWERKS__
void qsort(void *base, unsigned long n, unsigned long size, int (*cmp)(const void *, const void *));
#endif
int spawn_effect(struct Effect *effect);
void overwrite_effect_funcs(int type, struct EffectFuncs *funcs);

struct GMAModel *lbl_10_bss_58[53];
static struct GMAModel *lbl_10_bss_12C;
static struct GMAModel *lbl_10_bss_130;
static s16 *lbl_10_bss_134;
static struct GMAModel *lbl_10_bss_138;
static u32 s_perfTick;
static float s_debugPathRate;
static float s_debugPathWidth;
struct GMAModel *lbl_10_bss_148[8];
static Vec lbl_10_bss_168;
static struct ShadowCastReq lbl_10_bss_174;
struct Race2EffectLink lbl_10_bss_1D8[512];

#define PERF_END(i) lbl_1_bss_D418C[i] += (u32)((OSGetTick() - s_perfTick) * 8) / (*(u32 *)0x800000F8 / 4 / 125000)

void fn_10_A78C(void);
struct GMAModel *fn_10_A8E8(struct GMA *gma, char *name);
struct GMAModel *fn_10_A988(char *name);
void fn_10_AA68(void);
s32 fn_10_C358(const void *arg0, const void *arg1);
void fn_10_C3E8(void);
void fn_10_C4D0(struct Ball *ball);
void fn_10_C550(struct Ball *ball);
void fn_10_C5B8(void);
void fn_10_C69C(void);
void fn_10_C894(struct Ball *ball);
void fn_10_C9E0(void);
s32 fn_10_CE68(Vec *pos, float *out);
s32 fn_10_CF6C(Vec *pos, float *out);
void fn_10_D090(struct Ball *ball, Vec *pos, float scale, s16 rot);
void fn_10_D208(struct Camera *camera, Vec *pos, float x, float y);
void fn_10_D288(void);
void fn_10_D4BC(void);
void fn_10_D6F0(void);
void fn_10_D838(void);
void fn_10_DA50(struct Ball *ball);
void fn_10_DB84(struct Ball *ball);
void fn_10_DD4C(void);
void fn_10_DDA8(struct Effect *effect, struct Race2EffectLink *link);
void fn_10_DE50(void);
void fn_10_DEF0(void);
void fn_10_DF84(void);

/* unreferenced tables */
u32 lbl_10_data_9D0[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D,
    0x0E0FFF00, 0x00010203, 0x04050607, 0x08090A0B,
    0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000,
    0x10004C00, 0x00030000, 0x01000000, 0x00040000,
    0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000,
    0x18000000, 0x00050000, 0x40000000, 0x00000000,
    0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000,
    0x18000000, 0x00140000, 0x40000000, 0x00000000,
    0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000,
    0x40000000, 0x00000000, 0x01000000, 0x00110000,
    0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000,
    0x18000000, 0x00020000, 0x18000000, 0x00030000,
    0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000,
    0x18000000, 0x00080000, 0x18000000, 0x00090000,
    0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000,
    0x18000000, 0x000E0000, 0x18000000, 0x000F0000,
};

static s16 s_playerIconIds1P[8] = { 1, 3, 5, 7, 9, 9, 9, 9 };
static s16 s_playerIconIds2P[8] = { 2, 4, 6, 8, 10, 10, 10, 10 };

void fn_10_A78C(void)
{
    char *names[53] = {
        "MAP_ARROW",
        "MAP_ARROW_BASE",
        "HUNTER_MISSILE",
        "ITEM_A",
        "ITEM_B",
        "ITEM_C",
        "ITEM_D",
        "ITEM_E",
        "ITEM_F",
        "ITEM_G",
        "ITEM_H",
        "SPEED_SMOKE01",
        "SPEED_SMOKE02",
        "SPEED_SMOKE03",
        "SPEED_SMOKE04",
        "SPEED_SMOKE05",
        "SPEED_SMOKE06",
        "SPEED_SMOKE07",
        "SPEED_SMOKE08",
        "SPEED_SMOKE09",
        "SPEED_SMOKE10",
        "SPEED_SMOKE11",
        "SPEED_SMOKE12",
        "SPEED_SMOKE13",
        "SPEED_SMOKE14",
        "SPEED_SMOKE15",
        "SPEED_SMOKE16",
        "SPEED_SMOKE17",
        "SPEED_SMOKE18",
        "SPEED_SMOKE19",
        "SPEED_SMOKE20",
        "SPEED_SMOKE21",
        "SPEED_SMOKE22",
        "SPEED_SMOKE23",
        "SPEED_SMOKE24",
        "SPEED_SMOKE25",
        "SPEED_SMOKE26",
        "SPEED_SMOKE27",
        "SPEED_SMOKE28",
        "SPEED_SMOKE29",
        "SPEED_SMOKE30",
        "SPEED_SMOKE31",
        "SPEED_SMOKE32",
        "SPEED_SMOKE32",
        "G_BOM01",
        "G_BOM02",
        "G_BOM03",
        "G_BOM04",
        "G_BOM05",
        "G_BOM06",
        "G_BOM07",
        "G_BOM08",
        "G_BOM08",
    };
    s16 i;

    for (i = 0; i < 53; i++) {
        if ((lbl_10_bss_58[i] = fn_10_A8E8(minigameGma, names[i])) == NULL) {
            lbl_10_bss_58[i] = init_common_gma->modelEntries[0x25].model;
        }
    }
    lbl_10_bss_12C = fn_10_A988("_START_SIGN");
    lbl_10_bss_130 = fn_10_A988("_GOAL_SIGN");
    if (modeCtrl.unk30 >= 2) {
        lbl_10_bss_134 = s_playerIconIds2P;
        lbl_10_bss_138 = init_common_gma->modelEntries[0xA].model;
    } else {
        lbl_10_bss_134 = s_playerIconIds1P;
        lbl_10_bss_138 = init_common_gma->modelEntries[0x9].model;
    }
}

struct GMAModel *fn_10_A8E8(struct GMA *gma, char *name)
{
    struct GMAModel *model = NULL;
    s16 len = strlen(name);
    s16 i;
    char *modelName;

    for (i = 0; i < (s32)gma->numModels; i++) {
        modelName = gma->modelEntries[i].name;
        if (strncmp(name, modelName, len) == 0) {
            model = gma->modelEntries[i].model;
            break;
        }
    }
    return model;
}

struct GMAModel *fn_10_A988(char *name)
{
    struct GMAModel *model = NULL;
    s16 len = strlen(name);
    s16 i;
    s16 tmp;
    s16 diff;
    char *modelName;

    for (i = 0; i < (s32)stage_gma->numModels; i++) {
        modelName = stage_gma->modelEntries[i].name;
        tmp = strlen(modelName);
        diff = tmp - len;
        if (diff >= 0 && strcmp(name, modelName + diff) == 0) {
            model = stage_gma->modelEntries[i].model;
            break;
        }
    }
    return model;
}

void fn_10_AA68(void)
{
    Vec points[1000];
    struct RacePlayer *sorted[8];
    Vec prev;
    Vec cur;
    Vec a;
    Vec b;
    Vec w;
    float rate;
    float t;
    float f31;
    float f30;
    float f29;
    float f28;
    struct Ball *ball;
    struct Ball *savedBall;
    struct RacePlayer *self;
    struct RacePlayer *p;
    struct StageExtraChannels *chan;
    s32 i;
    s32 j;
    s16 k;
    s16 m;
    int n;
    s16 idx;
    s16 prevIdx;
    s16 nextIdx;
    s8 status;

    savedBall = current_ball;
    for (i = 0; i < 13; i++) {
        lbl_1_bss_D418C[i] = 0;
    }
    fn_10_D6F0();
    for (i = 0; i < 5; i++) {
        status = g_poolInfo.cameraPool.statusList[i];
        if (status == 0 || status == 4 || !(cameras[i].sub28.viewportW > 0.0f) || !(cameras[i].sub28.viewportH > 0.0f)) {
            continue;
        }
        if (i < modeCtrl.unk30) {
            current_ball = &balls[i];
            if (current_ball->status != 2) {
                continue;
            }
            self = (struct RacePlayer *)current_ball->unk150;
        } else {
            current_ball = balls;
            if (current_ball->status != 2) {
                continue;
            }
            self = (struct RacePlayer *)current_ball->unk150;
        }
        camera_apply_viewport_if_changed(i);
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(i);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(4);
        }
        PERF_END(0);
        s_perfTick = OSGetTick();
        PERF_END(1);
        s_perfTick = OSGetTick();
        if (lbl_1_bss_D4188 & 4) {
            status = events[EVENT_STAGE].state;
            if (status == 2 || status == 4) {
                g_draw_stage();
                if (stagedef->extraData != NULL) {
                    fn_10_C69C();
                }
            }
        }
        PERF_END(2);
        s_perfTick = OSGetTick();
        if (lbl_1_bss_D4188 & 8) {
            draw_shadow_reqs();
        }
        PERF_END(3);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x10) && events[EVENT_BACKGROUND].state == 2) {
            g_smth_with_rendefc_reflective_height(400.0f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        PERF_END(4);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x1000) && (status == 2 || status == 4)) {
            fn_10_C5B8();
        }
        PERF_END(12);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(0x10);
        }
        PERF_END(0);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x20) && events[EVENT_ITEM].state == 2) {
            draw_items();
        }
        PERF_END(5);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x40) && events[EVENT_STOBJ].state == 2) {
            g_draw_stobjs();
        }
        PERF_END(6);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x80) && events[EVENT_EFFECT].state == 2) {
            fn_10_DE50();
            g_draw_effects();
        }
        PERF_END(7);
        s_perfTick = OSGetTick();
        if (lbl_1_bss_D4188 & 0x100) {
            ball = balls;
            p = lbl_10_bss_1230;
            for (j = 0; j < player_count; j++, ball++, p++) {
                if (ball->status == 2) {
                    if (p->flags & 8) {
                        ball->flags |= 0x10;
                    } else {
                        ball->flags &= ~0x10ULL;
                    }
                    ball->ape->flags &= ~0x20;
                }
            }
            if (modeCtrl.unk30 >= 2 && player_count >= 5) {
                switch (modeCtrl.unk30) {
                case 2:
                    n = 5;
                    break;
                case 3:
                    if (modeCtrl.splitscreenMode == 3) {
                        n = 4;
                    } else {
                        n = 5;
                    }
                    break;
                case 4:
                    n = 4;
                    break;
                }
                mtxa_from_mtxb();
                for (j = 0; j < player_count; j++) {
                    if (g_is_sphere_visible(&balls[j].pos, balls[j].currRadius) == 0) {
                        lbl_10_bss_1230[j].unk320[g_current_camera->idx] = 0.0f;
                    }
                    sorted[j] = &lbl_10_bss_1230[j];
                }
                qsort(sorted, player_count, sizeof(sorted[0]), fn_10_C358);
                for (j = (player_count < n ? player_count : n) - 1; j >= 0; j--) {
                    ball = &balls[sorted[j]->unk0];
                    p = &lbl_10_bss_1230[ball->playerId];
                    if (0.0 <= p->unk320[g_current_camera->idx] && p->unk320[g_current_camera->idx] < 10.0) {
                        ball->ape->flags |= 0x20;
                    }
                }
                for (j = n; j < player_count; j++) {
                    ball = &balls[sorted[j]->unk0];
                    ball->ape->flags |= 0x20;
                }
            }
            if (events[EVENT_BALL].state == 2) {
                ball = balls;
                p = lbl_10_bss_1230;
                for (m = 0; m < player_count; m++, ball++, p++) {
                    if (!(g_current_camera->flags & 0x40) && (lbl_10_bss_1230[current_ball->playerId].flags & 0x40000)) {
                        if (!(p->flags & 0x40000)) {
                            ball->flags |= 0x10;
                            ball->ape->flags |= 0x20;
                        }
                    } else if (p->flags & 0x40000) {
                        ball->flags |= 0x10;
                        ball->ape->flags |= 0x20;
                    }
                }
                g_draw_ball_and_ape();
                fn_10_C3E8();
                fn_10_C9E0();
                if (!(g_current_camera->flags & 0x40) && !(self->flags & 2) && (self->flags & 4)) {
                    fn_10_C894(current_ball);
                }
            }
        }
        PERF_END(8);
        handle_test_camera();
        if ((dip_switches & 1) && (lbl_1_bss_D3360 & 8)) {
            chan = &stagedef->extraData->list[lbl_1_bss_D3364];
            idx = fn_1_3F61C("MonkeyRace2 CPU");
            prevIdx = idx - 1;
            nextIdx = idx + 1;
            f31 = -1.0f;
            f30 = -1.0f;
            f29 = -1.0f;
            if (idx >= 0) {
                f31 = lbl_1_bss_D3388[idx][0];
                if (prevIdx < 0) {
                    for (j = 0; j < 256; j++) {
                        if (lbl_1_bss_D3388[j][0] < 0.0) {
                            prevIdx = j - 1;
                            break;
                        }
                    }
                }
                if (prevIdx >= 0) {
                    f30 = lbl_1_bss_D3388[prevIdx][0];
                } else {
                    f30 = -1.0f;
                }
                if (nextIdx >= 256 || lbl_1_bss_D3388[nextIdx][0] < 0.0) {
                    nextIdx = 0;
                }
                f29 = lbl_1_bss_D3388[nextIdx][0];
            }
            if (f30 < 0.0) {
                f30 = 0.0f;
            }
            if (f31 < 0.0) {
                f31 = 0.0f;
            }
            if (f29 < 0.0) {
                f29 = 100.0f;
            }
            if (f31 < f30) {
                f30 = f31;
            }
            if (f31 > f29) {
                f29 = f31;
            }
            mtxa_from_mtxb();
            fn_10_181EC((struct StageExtraData *)chan, 0.0f, &prev);
            f28 = 0.1f;
            fn_1_E2FD0(0x1E);
            while (1) {
                if (f28 > f30) {
                    f28 = f30;
                }
                fn_10_181EC((struct StageExtraData *)chan, f28, &cur);
                fn_1_E3014(&prev, &cur, &(GXColor){ 0x64, 0x64, 0x64, 0xFF });
                prev = cur;
                if (f28 == f30) {
                    break;
                }
                f28 += 0.1;
            }
            fn_1_E2FD0(0x32);
            while (1) {
                if (f28 > f31) {
                    f28 = f31;
                }
                fn_10_181EC((struct StageExtraData *)chan, f28, &cur);
                fn_1_E3014(&prev, &cur, &(GXColor){ 0x32, 0x32, 0xFF, 0xFF });
                prev = cur;
                if (f28 == f31) {
                    break;
                }
                f28 += 0.1;
            }
            while (1) {
                if (f28 > f29) {
                    f28 = f29;
                }
                fn_10_181EC((struct StageExtraData *)chan, f28, &cur);
                fn_1_E3014(&prev, &cur, &(GXColor){ 0xFF, 0x32, 0x32, 0xFF });
                prev = cur;
                if (f28 == f29) {
                    break;
                }
                f28 += 0.1;
            }
            fn_1_E2FD0(0x1E);
            while (1) {
                if (f28 > 100.0) {
                    f28 = 100.0f;
                }
                fn_10_181EC((struct StageExtraData *)chan, f28, &cur);
                fn_1_E3014(&prev, &cur, &(GXColor){ 0x64, 0x64, 0x64, 0xFF });
                prev = cur;
                if (100.0 == f28) {
                    break;
                }
                f28 += 0.1;
            }
        }
        if ((dip_switches & 1) && (lbl_1_bss_D3360 & 3) && stagedef->extraData != NULL) {
            fn_1_E2FD0(0x1E);
            mtxa_from_mtxb();
            if (lbl_1_bss_D3360 & 1) {
                struct StageExtraData *data = stagedef->extraData;
                for (k = 0; k < 1000; k++) {
                    fn_10_181EC(data, 100.0 * ((float)k / 1000.0f), &points[k]);
                }
                fn_1_E3164(1000, points, &(GXColor){ 0xFF, 0x64, 0x64, 0xFF });
            }
            if (lbl_1_bss_D3360 & 2) {
                if (lbl_1_bss_D3368 < 0) {
                    lbl_1_bss_D3368 = 0;
                } else if (lbl_1_bss_D3368 >= stagedef->extraData->listCount) {
                    lbl_1_bss_D3368 = stagedef->extraData->listCount - 1;
                }
                chan = &stagedef->extraData->list[lbl_1_bss_D3368];
                for (k = 0; k < 1000; k++) {
                    fn_10_181EC((struct StageExtraData *)chan, 100.0 * ((float)k / 1000.0f), &points[k]);
                }
                fn_1_E3164(1000, points, &(GXColor){ 0x64, 0x64, 0xFF, 0xFF });
            }
            if (lbl_1_bss_D3360 & 4) {
                rate = 0.0f;
                if (pad_status_groups[0].unk0[0].button & 2) {
                    rate = 0.01f;
                } else if (pad_status_groups[0].unk0[0].button & 1) {
                    rate = -0.01f;
                }
                if (pad_status_groups[0].unk0[0].button & 0x20) {
                    rate *= 10.0;
                }
                s_debugPathRate += rate;
                t = 100.0 * s_debugPathRate;
                s_debugPathRate = (s32)t;
                if (t - s_debugPathRate >= 0.5) {
                    s_debugPathRate += 1.0;
                }
                s_debugPathRate *= 0.01;
                if (s_debugPathRate < 0.0) {
                    s_debugPathRate = 0.0f;
                } else if (s_debugPathRate > 100.0) {
                    s_debugPathRate = 100.0f;
                }
                if (lbl_1_bss_D3360 & 1) {
                    fn_10_181EC(stagedef->extraData, s_debugPathRate, &a);
                    b = (Vec){ a.x, 3.0 + a.y, a.z };
                    fn_1_E3014(&a, &b, &(GXColor){ 0xFF, 0x64, 0x64, 0xFF });
                }
                if (lbl_1_bss_D3360 & 2) {
                    fn_10_181EC((struct StageExtraData *)&stagedef->extraData->list[lbl_1_bss_D3368], s_debugPathRate, &a);
                    b = (Vec){ a.x, 3.0 + a.y, a.z };
                    fn_1_E3014(&a, &b, &(GXColor){ 0x64, 0x64, 0xFF, 0xFF });
                }
                debug_textdraw_set_pos(1, 0xE);
                debug_textdraw_vsprintf_buf_1("path rate:%6.2f\n", s_debugPathRate);
                if (lbl_1_bss_D3360 & 1) {
                    rate = 0.0f;
                    if (pad_status_groups[0].unk0[0].button & 8) {
                        rate = 0.01f;
                    } else if (pad_status_groups[0].unk0[0].button & 4) {
                        rate = -0.01f;
                    }
                    if (pad_status_groups[0].unk0[0].button & 0x20) {
                        rate *= 10.0;
                    }
                    s_debugPathWidth += rate;
                    if (s_debugPathWidth < 0.0) {
                        s_debugPathWidth = 0.0f;
                    }
                    t = 100.0 * s_debugPathWidth;
                    s_debugPathWidth = t;
                    if (t - s_debugPathWidth >= 0.5) {
                        s_debugPathWidth += 1.0;
                    }
                    s_debugPathWidth *= 0.01;
                    fn_10_181EC(stagedef->extraData, s_debugPathRate, &a);
                    fn_10_181EC(stagedef->extraData, 0.001 + s_debugPathRate, &w);
                    PSVECSubtract(&w, &a, &w);
                    vec_set_len(s_debugPathWidth, &w, &w);
                    mtxa_push();
                    mtxa_from_rotate_y(0x4000);
                    mtxa_tf_vec(&w, &w);
                    mtxa_pop();
                    a.y += 1.5;
                    PSVECAdd(&a, &w, &b);
                    PSVECSubtract(&a, &w, &a);
                    fn_1_E3014(&a, &b, &(GXColor){ 0xFF, 0x64, 0x64, 0xFF });
                    debug_textdraw_vsprintf_buf_1("width    :%6.2f\n", s_debugPathWidth);
                }
            }
        }
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x200) && (world_theme.unk8 & 1)) {
            fn_1_D6F9C(i);
        }
        PERF_END(9);
        s_perfTick = OSGetTick();
        if (lbl_1_bss_D4188 & 0x400) {
            ord_tbl_draw_nodes();
        }
        PERF_END(10);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x80) && events[EVENT_EFFECT].state == 2) {
            fn_10_DEF0();
            g_draw_effects();
            fn_10_DF84();
        }
        PERF_END(7);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 0x800) && (world_theme.unk8 & 1)) {
            fn_1_D6D84(i);
        }
        PERF_END(11);
        s_perfTick = OSGetTick();
        if ((lbl_1_bss_D4188 & 1) && events[EVENT_REND_EFC].state == 2) {
            g_something_with_rend_efc(8);
        }
        PERF_END(0);
    }
    current_ball = savedBall;
    camera_setup_default_projection();
}

s32 fn_10_C358(const void *arg0, const void *arg1)
{
    struct RacePlayer *p0 = *(struct RacePlayer **)arg0;
    struct RacePlayer *p1 = *(struct RacePlayer **)arg1;
    float a = p0->unk320[g_current_camera->idx];
    float b = p1->unk320[g_current_camera->idx];

    if (b < 0.0f) {
        return 1;
    }
    if (a < 0.0f) {
        return -1;
    }
    if (a < b) {
        return 1;
    }
    return -1;
}

void fn_10_C3E8(void)
{
    struct Ball *ball;
    s16 i;
    struct RacePlayer *player;

    i = 0;
    ball = balls;
    for (; i < 8; i++, ball++) {
        if (ball->status == 2) {
            player = (struct RacePlayer *)ball->unk150;
            if (!(player->flags & 0x20000) && (player->flags & 8) && (!(player->flags & 0x10) || !(g_font_char_var_3 & 4))) {
                if (player->unk2C2 & 4) {
                    fn_10_C4D0(ball);
                }
                if (player->unk2C2 & 8) {
                    fn_10_C550(ball);
                }
            }
        }
    }
}

void fn_10_C4D0(struct Ball *ball)
{
    Mtx mtx;

    mtxa_from_quat(&((struct RacePlayer *)ball->unk150)->unk2D4);
    mtxa_to_mtx(mtx);
    mtxa_from_mtxb_translate(&ball->pos);
    mtxa_mult_right(mtx);
    mtxa_scale_s(ball->modelScale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[0x14].model);
}

void fn_10_C550(struct Ball *ball)
{
    mtxa_from_mtxb();
    mtxa_mult_right(ball->unk30);
    mtxa_scale_s(ball->modelScale);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[0x15].model);
}

void fn_10_C5B8(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    struct RacePlayer *player = (struct RacePlayer *)current_ball->unk150;

    if (course->unk4 - 1 <= player->unk44 && __fabs(player->unk210) > 50.0) {
        if (lbl_10_bss_130 != NULL) {
            mtxa_from_mtxb();
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_never(lbl_10_bss_130);
        }
    } else if (lbl_10_bss_12C != NULL) {
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_never(lbl_10_bss_12C);
    }
}

void fn_10_C69C(void)
{
    struct StageExtraData *data = stagedef->extraData;
    struct Race2Marker *a = (struct Race2Marker *)data->unk20.frames;
    struct Race2Marker *b = (struct Race2Marker *)data->unk30.frames;
    s32 i;

    if (((struct RacePlayer *)current_ball->unk150)->flags & 0x40000) {
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        for (i = 0; i < 8; i++) {
            if (lbl_10_bss_148[i] != NULL) {
                avdisp_draw_model_culled_sort_auto(lbl_10_bss_148[i]);
            }
        }
    }
    for (i = 0; i < data->unk20.count; i++, a++) {
        if (modeCtrl.unk30 < 2 || !(mathutil_vec_sq_distance(&g_current_camera->eye, (Vec *)a) > 4900.0)) {
            mtxa_from_mtxb_translate((Vec *)a);
            mtxa_rotate_z(a->rotZ);
            mtxa_rotate_y(a->rotY);
            mtxa_rotate_x(a->rotX);
            avdisp_draw_model_culled_sort_always(minigameGma->modelEntries[0xA].model);
        }
    }
    for (i = 0; i < data->unk30.count; i++, b++) {
        mtxa_from_mtxb_translate((Vec *)b);
        mtxa_rotate_z(b->rotZ);
        mtxa_rotate_y(b->rotY);
        mtxa_rotate_x(b->rotX);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(minigameGma->modelEntries[0x8].model);
    }
}

void fn_10_C894(struct Ball *ball)
{
    struct RacePlayer *player = (struct RacePlayer *)ball->unk150;
    u16 val = 0;
    s16 i;

    if (!(player->unk2BA & 8)) {
        for (i = 2; i >= 0; i--) {
            if (player->unk2C8[i] != 0) {
                val = player->unk2C8[i];
                break;
            }
        }
        for (i = 0; i < 10; i++) {
            if (val & (1 << i)) {
                val = i;
                break;
            }
        }
        if (i < 10) {
            mtxa_from_mtxb_translate_xyz(ball->pos.x, 0.01f + (ball->pos.y + ball->currRadius), ball->pos.z);
            mtxa_sq_from_identity();
            mtxa_scale_xyz(0.7f, 0.7f, 0.7f);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_always(lbl_10_bss_58[val + 3]);
        }
    }
}

void fn_10_C9E0(void)
{
    Vec tops[8];
    Vec bases[8];
    Vec pos;
    float s2;
    float s1;
    float scale;
    float f31;
    s16 rot;
    struct Ball *ball;
    Vec *top;
    Vec *base;
    s16 i;

    if ((g_current_camera->flags & 0x40) || !(lbl_10_bss_1230[current_ball->playerId].flags & 2)) {
        mtxa_from_mtxb();
        for (i = 0; i < 8; i++) {
            if (i != current_ball->playerId || (g_current_camera->flags & 0x40)) {
                ball = &balls[i];
                if (ball->status == 2) {
                    top = &tops[i];
                    *top = ball->pos;
                    top->y += ball->currRadius;
                    mtxa_tf_point(top, top);
                    mtxa_tf_point(&ball->pos, &bases[i]);
                }
            }
        }
        for (i = 0; i < 8; i++) {
            if (i != current_ball->playerId || (g_current_camera->flags & 0x40)) {
                ball = &balls[i];
                if (ball->status == 2 && ball->unk154 != 0xE) {
                    top = &tops[i];
                    base = &bases[i];
                    if ((g_current_camera->flags & 0x40) || (!(g_current_camera->flags & 0x40) && current_ball->unk154 >= 9 && current_ball->unk154 <= 0xE) || base->z < -4.0) {
                        fn_10_CE68(top, &s2);
                        fn_10_D090(ball, top, s2, 0);
                    } else if (base->z > -1.0) {
                        if (fn_10_CF6C(base, &s1) != 0) {
                            base->z -= 0.001 * ball->playerId;
                            fn_10_D090(ball, base, s1, -0x3800);
                        }
                    } else {
                        f31 = (base->z - -1.0) / -3.0;
                        if (f31 > 1.0) {
                            f31 = 1.0f;
                        } else if (f31 < 0.0) {
                            f31 = 0.0f;
                        }
                        f31 = (f31 * (f31 * f31)) * (f31 * (f31 * f31));
                        if (fn_10_CF6C(base, &s1) != 0) {
                            base->z -= 0.001 * ball->playerId;
                            fn_10_CE68(top, &s2);
                            PSVECSubtract(top, base, &pos);
                            PSVECScale(&pos, &pos, f31);
                            PSVECAdd(&pos, base, &pos);
                            scale = s1 + f31 * (s2 - s1);
                            rot = 14336.0 * (1.0 - f31);
                            rot = -rot;
                        } else {
                            pos = *top;
                            fn_10_CE68(&pos, &scale);
                            rot = 0;
                        }
                        fn_10_D090(ball, &pos, scale, rot);
                    }
                }
            }
        }
    }
}

s32 fn_10_CE68(Vec *pos, float *out)
{
    float h = 240.0f * g_current_camera->sub28.viewportH;
    float inv = 1.0 / h;
    float dist = g_current_camera->sub28.view.tanHalfFov * mathutil_vec_len(pos);
    float size = h / dist;

    if (size < 20.0) {
        *out = 20.0 * dist * inv;
    } else if (size > 70.0f) {
        *out = 70.0 * dist * inv;
    } else {
        *out = 1.0f;
    }
    return 1;
}

s32 fn_10_CF6C(Vec *pos, float *out)
{
    double v;
    double halfW;
    float x;
    float y;

    if (pos->z > 20.0) {
        return 0;
    }
    *out = 0.3 + (1.0 - (pos->z / 20.0));
    if (*out > 1.0) {
        v = 1.0;
    } else {
        v = *out;
    }
    *out = 0.03 * v;
    halfW = 0.5 * (g_current_camera->sub28.viewportW * lbl_1_bss_2434);
    x = halfW * (1.0 + (pos->x * g_current_camera->sub28.view.tanHalfFov));
    y = (480.0f * g_current_camera->sub28.viewportH) - 10.0;
    pos->z = -0.15f;
    fn_10_D208(g_current_camera, pos, x, y);
    return 1;
}

void fn_10_D090(struct Ball *ball, Vec *pos, float scale, s16 rot)
{
    float *color;

    mtxa_from_identity();
    mathutil_mtxA_set_translate(pos);
    mtxa_rotate_x(rot);
    mtxa_scale_s(scale);
    avdisp_set_scale_factor(scale);
    if (ball->flags & (1ULL << 43)) {
        color = lbl_1_data_2E418[ball->playerId];
        set_post_mult_color(color[0], color[1], color[2], 1.0f);
        avdisp_draw_model_unculled_sort_always(lbl_10_bss_138);
        set_post_mult_color(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        avdisp_draw_model_unculled_sort_always(init_common_gma->modelEntries[lbl_10_bss_134[ball->playerId]].model);
    }
}

void fn_10_D208(struct Camera *camera, Vec *pos, float x, float y)
{
    float w = 640.0f * camera->sub28.viewportW;
    float h = 480.0f * camera->sub28.viewportH;
    float scale = (1.0 / (0.5 * h)) * (-pos->z * camera->sub28.view.tanHalfFov);

    pos->x = scale * (x - (0.5 * w));
    pos->y = scale * -(y - (0.5 * h));
}

void fn_10_D288(void)
{
    s32 i;
    struct Ball *ball = current_ball;
    s8 status;

    for (i = 0; i < 5; i++) {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f) {
            status = g_poolInfo.cameraPool.statusList[i];
            if (status != 0 && status != 4) {
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(i);
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(4);
                }
                g_something_with_drawing_apes();
                status = events[EVENT_STAGE].state;
                if (status == 2 || status == 4) {
                    g_draw_stage();
                }
                draw_shadow_reqs();
                if (events[EVENT_BACKGROUND].state == 2) {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(0x10);
                }
                if (events[EVENT_STOBJ].state == 2) {
                    g_draw_stobjs();
                }
                if (events[EVENT_EFFECT].state == 2) {
                    g_draw_effects();
                }
                handle_test_camera();
                ord_tbl_draw_nodes();
                if (world_theme.unk8 & 1) {
                    fn_1_D6D84(i);
                }
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(8);
                }
            }
        }
    }
    current_ball = ball;
    camera_setup_default_projection();
}

void fn_10_D4BC(void)
{
    s32 i;
    struct Ball *ball = current_ball;
    s8 status;

    for (i = 0; i < 5; i++) {
        if (cameras[i].sub28.viewportW > 0.0f && cameras[i].sub28.viewportH > 0.0f) {
            status = g_poolInfo.cameraPool.statusList[i];
            if (status != 0 && status != 4) {
                current_ball = &balls[i];
                camera_apply_viewport_if_changed(i);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(i);
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(4);
                }
                status = events[EVENT_STAGE].state;
                if (status == 2 || status == 4) {
                    g_draw_stage();
                }
                fn_1_120AD8();
                draw_shadow_reqs();
                if (events[EVENT_BACKGROUND].state == 2) {
                    g_smth_with_rendefc_reflective_height(400.0f);
                    g_draw_bg();
                    g_smth_with_rendefc_reflective_height(0.0f);
                }
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(0x10);
                }
                if (events[EVENT_STOBJ].state == 2) {
                    g_draw_stobjs();
                }
                if (events[EVENT_EFFECT].state == 2) {
                    g_draw_effects();
                }
                handle_test_camera();
                ord_tbl_draw_nodes();
                if (world_theme.unk8 & 1) {
                    fn_1_D6D84(i);
                }
                if (events[EVENT_REND_EFC].state == 2) {
                    g_something_with_rend_efc(8);
                }
            }
        }
    }
    current_ball = ball;
    camera_setup_default_projection();
}

void fn_10_D6F0(void)
{
    struct Race2Course *course = &lbl_10_bss_1208;
    s16 i;
    struct Ball *ball;

    fn_10_D838();
    if (course->unk2 & 0x10) {
        fn_10_DA50(balls);
    } else if (modeCtrl.unk30 == 1) {
        if (course->unk2 & 0x40) {
            i = 0;
            ball = balls;
            for (; i < player_count; i++, ball++) {
                fn_10_DB84(ball);
            }
        } else {
            fn_10_DA50(balls);
            i = 1;
            ball = &balls[1];
            for (; i < player_count; i++, ball++) {
                fn_10_DB84(ball);
            }
        }
    } else {
        i = 0;
        ball = balls;
        for (; i < player_count; i++, ball++) {
            fn_10_DB84(ball);
        }
    }
}

/* GXColor (a u32 table read as GXColor gave a byte-swapped colour on little endian) */
static GXColor lbl_10_data_E5C[10] = {
    { 0x47, 0x5F, 0x5F, 0xFF }, { 0x5F, 0x57, 0x47, 0xFF }, { 0x47, 0x49, 0x5F, 0xFF },
    { 0x5F, 0x4B, 0x5F, 0xFF }, { 0x5F, 0x4B, 0x4B, 0xFF }, { 0x47, 0x5F, 0x4B, 0xFF },
    { 0x5C, 0x5A, 0x5A, 0xFF }, { 0x47, 0x47, 0x49, 0xFF }, { 0 }, { 0 },
};
static struct ShadowTexInfo lbl_10_data_E84 = { 1 };

void fn_10_D838(void)
{
    int width;
    int height;
    int fmt;
    int mipmap;
    GXTexObj *tex;

    mtxa_from_rotate_y(s_lightPool[0].rotY);
    mtxa_rotate_x(s_lightPool[0].rotX);
    lbl_10_bss_168.x = 0.0f;
    lbl_10_bss_168.y = 0.0f;
    lbl_10_bss_168.z = -15.0f;
    mtxa_tf_vec(&lbl_10_bss_168, &lbl_10_bss_168);
    lbl_10_bss_174.unk30 = 6;
    lbl_10_bss_174.unk34 = 0.1f;
    lbl_10_bss_174.unk38 = 0.2f;
    lbl_10_bss_174.up.x = 0.0f;
    lbl_10_bss_174.up.y = 1.0f;
    lbl_10_bss_174.up.z = 0.0f;
    mtxa_tf_vec(&lbl_10_bss_174.up, &lbl_10_bss_174.up);
    tex = init_common_gma->modelEntries[0x5E].model->texObjs;
    mipmap = GXGetTexObjMipMap(tex);
    fmt = GXGetTexObjFmt(tex);
    height = GXGetTexObjHeight(tex);
    width = GXGetTexObjWidth(tex);
    GXInitTexObj(&lbl_10_data_E84.texObj, GXGetTexObjData(tex), width, height, fmt, 0, 0, mipmap);
    GXInitTexObjLOD(&lbl_10_data_E84.texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    lbl_10_bss_174.tex = lbl_10_data_E84;
}

void fn_10_DA50(struct Ball *ball)
{
    if (ball->unk154 != 0xE) {
        lbl_10_bss_174.obj = ball;
        lbl_10_bss_174.color = lbl_10_data_E5C[ball->colorId];
        lbl_10_bss_174.lightPos.x = lbl_10_bss_168.x + ball->pos.x;
        lbl_10_bss_174.lightPos.y = lbl_10_bss_168.y + ball->pos.y;
        lbl_10_bss_174.lightPos.z = lbl_10_bss_168.z + ball->pos.z;
        lbl_10_bss_174.unk10 = 5.0f;
        lbl_10_bss_174.pos = ball->pos;
        lbl_10_bss_174.radius = 1.4f * ball->currRadius;
        lbl_10_bss_174.cameraMask = 0xFFFF;
        fn_1_D4650(&lbl_10_bss_174);
    }
}

void fn_10_DB84(struct Ball *ball)
{
    struct ShadowReq req;
    struct RaycastHit hit;
    Vec normal;
    float a;
    float t;
    u8 *color;

    if (ball->unk154 != 0xE) {
        req.unk20 = 0.025f;
        req.unk24 = 0.05f;
        req.model = init_common_gma->modelEntries[0x67].model;
        if ((s32)raycast_stage_down(&ball->pos, &hit, &normal) != 0) {
            a = 1.0 - (0.2 * (ball->pos.y - hit.pos.y));
            if (!(a < 0.0f)) {
                req.color = lbl_10_data_E5C[ball->colorId];
                req.color.r = (float)req.color.r * a;
                req.color.g = (float)req.color.g * a;
                req.color.b = (float)req.color.b * a;
                t = ball->currRadius * (1.0f + (2.0f * (1.0f - a)));
                req.scale.x = t;
                req.scale.y = req.scale.x;
                req.scale.z = req.scale.x;
                vec_to_euler(&hit.normal, &req.rot);
                req.rot.z = ball->rotZ;
                req.pos = hit.pos;
                submit_shadow_req(&req);
            }
        }
    }
}

void fn_10_E008(struct Effect *effect);
void fn_10_E0F4(struct Effect *effect);
void fn_10_E160(struct Effect *effect);
void fn_10_E360(struct Effect *effect);
void fn_10_E364(struct Effect *effect);
void fn_10_E440(struct Effect *effect);
void fn_10_E4C4(struct Effect *effect);
void fn_10_E598(struct Effect *effect);
void fn_10_E59C(struct Effect *effect);
void fn_10_E5BC(struct Effect *effect);
void fn_10_E7CC(struct Effect *effect);
void fn_10_E8F0(struct Effect *effect);
void fn_10_E8F4(struct Effect *effect);
void fn_10_E9F4(struct Effect *effect);
void fn_10_EB1C(struct Effect *effect);
void fn_10_EB98(struct Effect *effect);
void fn_10_EB9C(struct Effect *effect);
void fn_10_EC6C(struct Effect *effect);
void fn_10_ED94(struct Effect *effect);
void fn_10_EE10(struct Effect *effect);
void fn_10_EE14(struct Effect *effect);
void fn_10_EE70(struct Effect *effect);
void fn_10_F034(struct Effect *effect);
void fn_10_F15C(struct Effect *effect);
void fn_10_F160(struct Effect *effect);
void fn_10_F16C(struct Effect *effect);
void fn_10_F214(struct Effect *effect);
void fn_10_F460(struct Effect *effect);
void fn_10_F8B4(struct Effect *effect);
void fn_10_F9DC(struct Effect *effect);
void fn_10_FD2C(struct Effect *effect);
void fn_10_10000(struct Effect *effect);
void fn_10_10004(struct Effect *effect);
void fn_10_1007C(struct Effect *effect);
void fn_10_10308(struct Effect *effect);
void fn_10_103A4(struct Effect *effect);
void fn_10_103A8(struct Effect *effect);
void fn_10_103B4(struct Effect *effect);
void fn_10_105C4(struct Effect *effect);
void fn_10_10720(struct Effect *effect);

static struct EffectFuncs lbl_10_data_EA8[10] = {
    { fn_10_E008, fn_10_E0F4, fn_10_E160, fn_10_E360 },
    { fn_10_E364, fn_10_E440, fn_10_E4C4, fn_10_E598 },
    { fn_10_E59C, fn_10_E5BC, fn_10_E7CC, fn_10_E8F0 },
    { fn_10_E8F4, fn_10_E9F4, fn_10_EB1C, fn_10_EB98 },
    { fn_10_EB9C, fn_10_EC6C, fn_10_ED94, fn_10_EE10 },
    { fn_10_EE14, fn_10_EE70, fn_10_F034, fn_10_F15C },
    { fn_10_F160, fn_10_F16C, fn_10_F214, fn_10_F460 },
    { fn_10_F8B4, fn_10_F9DC, fn_10_FD2C, fn_10_10000 },
    { fn_10_10004, fn_10_1007C, fn_10_10308, fn_10_103A4 },
    { fn_10_103A8, fn_10_103B4, fn_10_105C4, fn_10_10720 },
};

void fn_10_DD4C(void)
{
    s16 i;

    for (i = 0; i < 10; i++) {
        overwrite_effect_funcs(i + 0x27, &lbl_10_data_EA8[i]);
    }
}

void fn_10_DDA8(struct Effect *effect, struct Race2EffectLink *link)
{
    s16 type = effect->type;
    s32 id;
    struct Effect *spawned;

    effect->type += 0x27;
    id = spawn_effect(effect);
    effect->type = type;
    if (id >= 0 && link != NULL) {
        spawned = fn_1_6876C(id);
        if (spawned->poolIndex >= 0 && spawned->poolIndex < 512) {
            lbl_10_bss_1D8[spawned->poolIndex] = *link;
        }
    }
}

void fn_10_DE50(void)
{
    s16 n;
    struct Effect *effect;
    s8 *status;

    effect = effects;
    status = g_poolInfo.effectPool.statusList;
    n = g_poolInfo.effectPool.count;
    while (n > 0) {
        if (*status != 0) {
            if (effect->flags & 1) {
                effect->flags |= 0x20000000;
            } else {
                effect->flags &= ~0x20000000;
                if (effect->flags & 0x10000000) {
                    effect->flags |= 1;
                }
            }
        }
        n--;
        status++;
        effect++;
    }
}

void fn_10_DEF0(void)
{
    s16 n;
    struct Effect *effect;
    s8 *status;

    effect = effects;
    status = g_poolInfo.effectPool.statusList;
    n = g_poolInfo.effectPool.count;
    while (n > 0) {
        if (*status != 0 && !(effect->flags & 0x20000000)) {
            if (effect->flags & 0x10000000) {
                effect->flags &= ~1;
            } else {
                effect->flags |= 1;
            }
        }
        n--;
        status++;
        effect++;
    }
}

void fn_10_DF84(void)
{
    s16 n;
    struct Effect *effect;
    s8 *status;

    effect = effects;
    status = g_poolInfo.effectPool.statusList;
    n = g_poolInfo.effectPool.count;
    while (n > 0) {
        if (*status != 0) {
            if (effect->flags & 0x20000000) {
                effect->flags |= 1;
            } else {
                effect->flags &= ~1;
            }
        }
        n--;
        status++;
        effect++;
    }
}

