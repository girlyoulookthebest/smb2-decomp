#ifndef CAMERA_H
#define CAMERA_H

#include "types.h"
#include "mathutil.h"

/* Projection parameters (camera_build_projection_for copies this as a unit). */
struct CameraView
{
    /*0x00*/ float unk28;  /* horizontal projection offset */
    /*0x04*/ float unk2C;  /* vertical projection offset */
    /*0x08*/ s16 fov;
    /*0x0A*/ s16 nextFov;
    /*0x0C*/ float aspect;
    /*0x10*/ float tanHalfFov;
    /*0x14*/ float invTanHalfFov;
    /*0x18*/ float nearZ;
    /*0x1C*/ float farZ;
}; /* size = 0x20 */

/* camera_clear preserves this block as a unit. */
struct CameraSub28
{
    /*0x00*/ struct CameraView view;
    /*0x20*/ float viewportX;
    /*0x24*/ float viewportY;
    /*0x28*/ float viewportW;
    /*0x2C*/ float viewportH;
}; /* size = 0x30 */

/* Offsets confirmed against matched code; unknown fields are padding until identified. */
struct Camera
{
    /*0x00*/ Vec eye;
    /*0x0C*/ Vec lookAt;
    /*0x18*/ s16 rotX;
    /*0x1A*/ s16 rotY;
    /*0x1C*/ s16 rotZ;
    /*0x1E*/ s8 mode;
    /*0x1F*/ s8 unk1F;
    /*0x20*/ float unk20;
    /*0x24*/ s16 flags;
    /*0x26*/ s8 unk26;
    /*0x28*/ struct CameraSub28 sub28;
    /*0x58*/ s16 timerCurr;
    /*0x5A*/ s16 timerMax;
    /* 0x5C-0x9B are per-mode scratch; names describe the first mode that used them. */
    /*0x5C*/ Vec unk5C;  /* menu: eye wobble amplitude; ready: fly-in start look-at */
    /*0x68*/ float unk68;  /* ready: start distance */
    /*0x6C*/ float unk6C;  /* ready: start height */
    /*0x70*/ s32 unk70;  /* menu: eye wobble frequencies (x, y, z) */
    /*0x74*/ s32 unk74;  /* stage orbit: yaw */
    /*0x78*/ s32 unk78;
    /*0x7C*/ Vec unk7C;  /* menu: look-at wobble amplitude */
    /*0x88*/ float unk88;  /* stage orbit: zoom; menu: wobble time */
    /*0x8C*/ float unk8C;
    /*0x90*/ s32 unk90;  /* menu: look-at wobble frequencies (x, y, z) */
    /*0x94*/ s32 unk94;  /* stage orbit: yaw speed */
    /*0x98*/ s32 unk98;
    /*0x9C*/ Vec eyeVel;
    /*0xA8*/ Vec lookAtVel;
    /*0xB4*/ Vec unkB4;  /* point the follow camera trails behind */
    /*0xC0*/ s16 unkC0;  /* smoothed pitch */
    u8 unkC2[0xC4 - 0xC2];
    /*0xC4*/ Vec unkC4;
    /*0xD0*/ Vec unkD0;
    u8 unkDC[0xF4 - 0xDC];
    /*0xF4*/ s32 shakeActive;
    /*0xF8*/ s32 shakeTimer;  /* frames left; <= 0 means until stopped */
    /*0xFC*/ Vec shakeOffset;
    /*0x108*/ Vec shakeVel;
    /*0x114*/ s16 unk114;  /* last yaw change */
    /*0x116*/ s16 unk116;  /* item group the replay camera is attached to */
    /*0x118*/ s16 unk118;  /* goal the replay camera is relative to, or -1 */
    u8 unk11A[0x11C - 0x11A];
    /*0x11C*/ Vec unk11C;  /* eye, in the attached frame */
    /*0x128*/ Vec unk128;
    /*0x134*/ Vec unk134;  /* look-at, in the attached frame */
    /*0x140*/ Vec unk140;  /* look-at velocity, in the attached frame */
    /*0x14C*/ Mtx unk14C;
    /*0x17C*/ Mtx unk17C;
    /*0x1AC*/ Mtx unk1AC;
    /*0x1DC*/ Mtx unk1DC;
    /*0x20C*/ s32 idx;
    /*0x210*/ s8 unk210;
    u8 unk211[0x28C - 0x211];
}; /* size = 0x28C */

struct Ball;

enum
{
    CAMERA_MODE_TEST = 2,
};

enum
{
    CAMERA_FLAG_UNK1 = 1 << 0,
    CAMERA_FLAG_UNK4 = 1 << 2,
    CAMERA_FLAG_NO_STAGE_TILT = 1 << 3,
    CAMERA_FLAG_UNK10 = 1 << 4,
    CAMERA_FLAG_UNK80 = 1 << 7,
};

extern void (*camera_funcs[])(struct Camera *, struct Ball *);

/* View matrices handed to the renderer (SMB1's lbl_802F1B3C). */
struct ViewMatrices
{
    Mtx matrices[5];
    /*0x0F0*/ struct OrdTblNode *entries;
    /*0x0F4*/ s32 maxEntries;
    /*0x0F8*/ float depthOffset;
    /*0x0FC*/ float minDepth;
    /*0x100*/ float maxDepth;
    /*0x104*/ struct OrdTblNode *lastEntry;
    /*0x108*/ struct OrdTblNode *firstEntry;
    /*0x10C*/ float depthRange;
};

extern struct ViewMatrices *g_ord_tbl_stuff;

/* camera.c .bss, in address order. */
extern s16 lbl_1_bss_1768;
extern s8 lbl_1_bss_176A;
extern s32 g_something_with_camera3;
extern s32 lbl_1_bss_1770;
extern struct Camera *g_current_camera;
extern struct Camera cameras[5];
extern float lbl_1_bss_2434;
extern float g_camera_aspect_ratio;
extern s8 lbl_1_bss_243C;
extern s8 g_related_to_fov2;
extern s16 lbl_1_bss_243E;  /* effective FOV of the last projection */
extern float lbl_1_bss_2440;  /* tan(effective FOV / 2) */
/* Per-camera state for the attract-mode demo camera (modes 44/45). */
struct DemoCameraState
{
    /*0x00*/ s32 subMode;  /* mode the demo camera is currently running */
    /*0x04*/ s32 lastSubMode;
    /*0x08*/ u8 replayHeader[0x18];  /* SMB1's ReplayHeader; flags (u16) at +0 */
}; /* size = 0x20 */

extern struct DemoCameraState demoCameraStates[5];

/* Camera pose fed in from outside the camera system (modes 84/85). */
/* View of scnplay.c's struct ScenePlayer (lbl_1_bss_DD4F4): the leading
 * fields are spelled out so the layout also holds on 64-bit hosts. */
struct ExternalCameraPose
{
    /*0x00*/ void *instances;
    /*0x04*/ s32 count;
    /*0x08*/ s32 stage;
    /*0x0C*/ int category;
    /*0x10*/ u16 sceneNum;
    /*0x12*/ u16 frame;  /* current cutscene frame */
    /*0x14*/ u16 frameMax;
    /*0x16*/ s16 rotZ;
    /*0x18*/ s16 fov;
    u8 unk1A[2];
    /*0x1C*/ Vec eye;
    /*0x28*/ Vec lookAt;
    u8 unk34[4];
};  /* size = 0x38 */

extern struct ExternalCameraPose lbl_1_bss_DD4F4;
extern void *g_minigame_camera_func;
extern s16 g_camera_standstill_counters[8];  /* per camera; frames the monkey has been idle, 0-60 */

void init_cameras(void);
void event_camera_init(void);
void event_camera_tick(void);
void event_camera_dest(void);
void enable_camera(int cameraId);
void disable_camera(int cameraId);
void disable_all_cameras(void);
void enable_cameras_for_active_players(void);
void camera_update_aspect_ratios(void);
float camera_get_aspect_ratio(void);
void camera_clear(struct Camera *camera);
void camera_setup_default_projection(void);
void set_camera_view_settings(int cameraId, float left, float top, float width, float height);
void camera_setup_splitscreen_viewports(int playerCount);
void camera_setup_singleplayer_viewport(void);
void camera_apply_viewport(int cameraId);
void camera_apply_viewport_if_changed(int cameraId);
void camera_apply_viewport_2(int cameraId);
void camera_apply_viewport_2_if_changed(int cameraId);
void camera_apply_main_projection(void);
void camera_start_shake(int cameraId, int duration, Vec *impulse);
void camera_set_state(int state);
void camera_set_state_for_mask(u32 mask, int state);
void camera_set_or_clear_flags(int flags, int set);
void camera_build_projection(Mtx44 mtx, int fov, float aspect, float nearZ, float farZ, float offsetX, float offsetY);
void camera_build_projection_for(Mtx44 mtx, struct Camera *camera, float nearZ, float farZ);
void camera_update_shake(struct Camera *camera);

#endif
