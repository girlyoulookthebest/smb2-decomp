/* The ending sequences played after clearing a difficulty. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "effect.h"
#include "ending.h"
#include "event.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "macros.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "stage.h"
#ifndef __MWERKS__
#include "frustum.h" /* prototypes: float args must not be promoted to double on x86 */
#endif

struct TextBox
{
    /*0x00*/ int state;
    u8 unk4[0xC - 0x4];
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    u8 unk10[0x15 - 0x10];
    /*0x15*/ s8 lines;
    /*0x16*/ u8 unk16;
    u8 unk17[0x1C - 0x17];
    /*0x1C*/ void (*callback)(struct TextBox *tb);
    u8 unk20[0x34 - 0x20];
}; /* size = 0x34 */

extern s32 active_monkey_id[];
extern int stage_heap;
extern u16 merged_digital_inputs[];
extern s16 current_stage_id;
extern s32 sub_mode;
extern struct Event events[];
extern struct GMA *g_bg_gma;

void dest_all_events(void);
void SoundGroupLoad(int group);
void set_sub_mode_destination(void (*func)(void));
void g_something_calls_sndPopGroup(void);
void fn_1_D4870(void);
void g_maybe_some_bg_effect_handler(void);
void g_draw_stage(void);
void g_draw_bg(void);
void g_draw_func_init(void);
void ord_tbl_draw_nodes(void);
void g_smth_with_rendefc_reflective_height(float height);
void load_stage(int stageId);
void event_init(int id);
void g_init_rendefc_for_stage(void);
void fade_screen_to_color(int a, int b, int c);
void g_fade_track_volume(int a, int b);
void call_SoundReqID_arg_0(int id);
void SoundReq(int id);
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *tb);

void g_clear_effect_ending_ballfrag(void);
void g_set_effect_ending_ballfrag(void);
void fn_1_1082D4(void);
void fn_1_108344(void);
void fn_1_1090E0(struct EndingMsgLines *lines, char *buf);
void fn_1_107CF8(int a);
void fn_1_107D78(void);
void fn_1_107DE8(void);
void fn_1_1075E0(void);
void fn_1_1075E4(void);
extern s32 lbl_1_bss_70E98[];
extern struct BgLightInfo lbl_1_bss_2F48;
void mtxa_rigid_inv_tf_vec_xyz(float x, float y, float z, Vec *dst);
void fn_1_107188(void);
void GXSetTevSwapMode_cached(int stage, int ras, int tex);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, u8 clear);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, u8 mipmap);
void GXSetTevKColor_cached(int id, GXColor color);
void GXSetTevKAlphaSel_cached(int stage, int sel);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_something_with_raster_state(int a);
void C_MTXLookAt(Mtx m, Vec *pos, Vec *up, Vec *target);
extern void *lbl_801988E0;
void fn_1_D29E0(int);
struct Ape *fn_1_478C4(int, int, void (*)(struct Ape *, int));
void fn_1_D1F28(struct Ape *ape);
void fn_1_D292C(void);
void fn_1_D1F88(struct Ape *ape, int a, int b, float c, int d);
void fn_1_D2510(struct Ape *ape);
void fn_1_107BD0(int a);
int raycast_stage_down(Vec *pos, struct RaycastHit *hit, Vec *c);
void fn_1_107A88(void);
void fn_1_107A18(void);
void fn_1_1053B0(void);
void fn_1_107864(void);
void fn_1_103D8C(int a);
void fn_1_107C64(int a, int b);
void fn_1_6869C(int a);

void fn_1_FD8D4(void);
void fn_1_103DF8(void);
void fn_1_1053B0(void);
void fn_1_1031D8(void);
void fn_1_103E44(void);
void fn_1_1053E4(void);
void fn_1_1031F8(void);
void fn_1_103CA8(void);
void fn_1_104BD4(void);
void fn_1_104C6C(void);
void fn_1_103CAC(void);
int fn_1_1050D8(int a, int b);
void fn_1_FDABC(void);
void fn_1_FDB24(void);
void fn_1_FDDB4(void);
void fn_1_FDFCC(void);
void fn_1_FE25C(void);
void fn_1_FE568(void);
void fn_1_FE698(void);
void fn_1_FE7AC(void);
void fn_1_FEA88(void);
void fn_1_FF178(void);
void fn_1_FF3BC(void);
void fn_1_FF650(void);
void fn_1_FF6AC(void);
void fn_1_FFB5C(void);
void fn_1_FFF04(void);
void fn_1_FFFCC(void);
void fn_1_1003EC(void);
void fn_1_100744(void);
void fn_1_1007EC(void);
void fn_1_101010(void);
void fn_1_101340(void);
void fn_1_101590(void);
void fn_1_101754(void);
void fn_1_101C64(void);
void fn_1_101E24(void);
void fn_1_101F78(void);
void fn_1_101FC4(void);
void fn_1_102914(void);
void fn_1_102948(void);
void fn_1_102400(void);
void fn_1_102434(void);
void fn_1_1031C0(void);
void fn_1_1031D4(void);

u32 lbl_1_data_63408[] = {
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

struct EndingCharaStart
{
    Vec pos;
    S16Vec rot;
};

struct EndingCharaStart lbl_1_data_63578[] = {
    { { 0.7f, -0.4f, 0.7f }, { 0, -0x8000, 0 } },
    { { -0.675f, 0.0f, 0.75f }, { 0, -0x8000, 0 } },
    { { 0.0200000014f, -0.4f, 1.1f }, { 0, -0x8000, 0 } },
};

Vec lbl_1_data_635B4[] = {
    { 0.0f, 10.0f, 5.5f },
    { -1.20000005f, 10.0f, 5.0f },
    { 1.20000005f, 10.0f, 5.0f },
};

Vec lbl_1_data_635D8[] = {
    { 0.0f, 10.0f, 4.0f },
    { -0.100000001f, 10.0f, 5.5f },
    { 0.100000001f, 10.0f, 6.5f },
    { 0.0f, 10.0f, 9.5f },
};

s16 lbl_1_data_63608[][2] = { { 0xE9, 0xE9 }, { 0x102, 0x102 }, { 0x16E, 0x16E }, { 0x187, 0x188 } };

void (*lbl_1_data_63618[])(void) = {
    fn_1_FDABC,
    fn_1_FDB24,
    fn_1_FDDB4,
    fn_1_FDFCC,
    fn_1_FE25C,
    fn_1_FE568,
    fn_1_FE698,
    fn_1_FE7AC,
    fn_1_FEA88,
    fn_1_FF178,
    fn_1_FF3BC,
    fn_1_FF650,
    fn_1_FF6AC,
    fn_1_FFB5C,
    fn_1_FFF04,
    fn_1_FFFCC,
    fn_1_1003EC,
    fn_1_100744,
    fn_1_1007EC,
    fn_1_101010,
    fn_1_101340,
    fn_1_101590,
    fn_1_101754,
    fn_1_101C64,
    fn_1_101E24,
    fn_1_101F78,
    fn_1_101FC4,
    fn_1_102914,
    fn_1_102948,
    fn_1_102400,
    fn_1_102434,
    fn_1_1031C0,
    fn_1_1031D4,
};

s32 lbl_1_bss_D3350;
struct EndingInfo lbl_1_bss_D3354;

FORCE_BSS_ORDER(lbl_1_bss_D3350)

void g_alloc_memory_for_ending(void)
{
    memset(&lbl_1_bss_D3354, 0, sizeof(lbl_1_bss_D3354));
    lbl_1_bss_D3354.state = 0;
    lbl_1_bss_D3354.nextState = -1;
    dest_all_events();
    lbl_1_bss_D3354.work = OSAllocFromHeap(stage_heap, sizeof(struct EndingWork));
    if (lbl_1_bss_D3354.work == NULL)
    {
        printf("fail to alloc memory for ending-management!\n");
        lbl_1_bss_D3354.state = 0x1F;
        return;
    }
    memset(lbl_1_bss_D3354.work, 0, sizeof(struct EndingWork));
    SoundGroupLoad(0x10);
    fn_1_103DF8();
    fn_1_1053B0();
    fn_1_1031D8();
    fn_1_1082D4();
    g_set_effect_ending_ballfrag();
    set_sub_mode_destination(fn_1_FD8D4);
}

s16 lbl_1_data_636CC[] = { 0xEB, 0x103, 0x143, 0x174 };
s16 lbl_1_data_636D4[] = { 0xB8, 0x103, 0x142, 0x18C };
s16 lbl_1_data_636DC[] = { 0xED, 0x116, 0x16E, 0x174, 0xB6, 0x100, 0x139, 0x193, 0xEB, 0x103, 0x16E, 0x174 };
s16 lbl_1_data_636F4[] = { 0xE5, 0x128, 0x14B, 0 };
s16 lbl_1_data_636FC[] = { 0xE9, 0x102, 0x16C, 0 };
s16 lbl_1_data_63704[] = { 0xE9, 0x103, 0x16E, 0 };

int fn_1_FD740(void)
{
    struct EndingCamera *camera;

    if (lbl_1_bss_D3354.work == NULL)
        return 0;
    if (g_some_other_flags & 0xA)
        return 1;
    if (lbl_1_bss_D3354.nextState != -1)
        lbl_1_bss_D3354.state = lbl_1_bss_D3354.nextState;
    lbl_1_data_63618[lbl_1_bss_D3354.state]();
    fn_1_103E44();
    fn_1_1053E4();
    fn_1_1031F8();
    if (lbl_1_bss_D3354.flags & 2)
        lbl_1_bss_D3354.work->timer += 0.016666668f;
    camera = &lbl_1_bss_D3354.work->camera;
    camera->eye.x += camera->eyeVel.x;
    camera->eye.y += camera->eyeVel.y;
    camera->eye.z += camera->eyeVel.z;
    camera->lookAt.x += camera->lookAtVel.x;
    camera->lookAt.y += camera->lookAtVel.y;
    camera->lookAt.z += camera->lookAtVel.z;
    return (s16)lbl_1_bss_D3354.state != (s16)0x20;
}

void fn_1_FD8D4(void)
{
    switch (sub_mode)
    {
    case 0x48:
    case 0x49:
    case 0xB7:
    case 0xB8:
        set_sub_mode_destination(fn_1_FD8D4);
        return;
    }
    g_clear_effect_ending_ballfrag();
    fn_1_108344();
    fn_1_103CA8();
    fn_1_1075E0();
    fn_1_104BD4();
    OSFreeToHeap(stage_heap, lbl_1_bss_D3354.work);
    lbl_1_bss_D3354.work = NULL;
    dest_all_events();
    g_something_calls_sndPopGroup();
}

void fn_1_FD97C(void)
{
    s8 stageState;

    if (lbl_1_bss_D3354.work == NULL)
        return;
    camera_apply_viewport_if_changed(modeCtrl.currPlayer);
    fn_1_D4870();
    g_maybe_some_bg_effect_handler();
    fn_1_1075E4();
    if (lbl_1_bss_D3354.flags & 1)
    {
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(g_bg_gma->modelEntries[4].model);
    }
    fn_1_104C6C();
    fn_1_103CAC();
    stageState = events[0].state;
    if (stageState == 2 || stageState == 4)
        g_draw_stage();
    if (events[20].state == 2)
    {
        g_smth_with_rendefc_reflective_height(400.0f);
        g_draw_bg();
        g_smth_with_rendefc_reflective_height(0.0f);
    }
    if (events[14].state == 2)
        g_draw_effects();
    g_draw_func_init();
    ord_tbl_draw_nodes();
    if (lbl_1_bss_D3354.flags & 0x20)
        fn_1_107DE8();
}

void fn_1_FDABC(void)
{
    switch (modeCtrl.difficulty)
    {
    case 0:
    default:
        lbl_1_bss_D3354.nextState = 1;
        break;
    case 1:
        lbl_1_bss_D3354.nextState = 7;
        break;
    case 2:
        lbl_1_bss_D3354.nextState = 0xD;
        break;
    }
}

#define RESET_ALL_CAMERAS(camera, curr, i) \
    curr = g_current_camera; \
    camera = cameras; \
    for (i = 0; i < 5; i++, camera++) \
    { \
        g_current_camera = camera; \
        camera->unk1F = 0; \
    } \
    g_current_camera = curr

#define SHOW_MSG_LINES(work, tb, i, kind, posY) \
    memset(&tb, 0, sizeof(tb)); \
    tb.unk16 = kind; \
    tb.x = 0x140; \
    tb.y = posY; \
    tb.lines = (work->msg.count < 2) ? work->msg.count : 2; \
    tb.callback = NULL; \
    g_display_playpoint_or_gift_message_child(0, 1, &tb); \
    for (i = 0; i < tb.lines; i++, work->msg.count--, work->msgLine++) \
        g_smth_with_playpoint_or_gift_msg(0, work->msg.lines[work->msgLine])

#define CHARA_TALK(chara) \
    chara->unk38--; \
    if (chara->unk38 < 0) \
    { \
        SoundReq(lbl_1_data_63608[chara->ape->charaId][chara->unk3A & 1]); \
        chara->unk38 = 240.0f * (0.8f + 0.4f * (rand() / 32767.0f)); \
        chara->unk3A++; \
    }

void fn_1_FDB24(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    Vec v;
    int stageId;

    lbl_1_bss_D3354.nextState = 2;
    modeCtrl.submodeTimer = 0x1A4;
    dest_all_events();
    switch (modeCtrl.difficulty)
    {
    case 0:
        stageId = 0xC6;
        break;
    case 1:
        stageId = 0xC6;
        break;
    case 2:
    default:
        stageId = 0xC6;
        break;
    }
    load_stage(stageId);
    event_init(0);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(0xB);
    light_init(current_stage_id);
    g_init_rendefc_for_stage();
    fn_1_1050D8(0, active_monkey_id[modeCtrl.currPlayer]);
    lbl_1_bss_D3354.work->charas[0].unk70 = 1;
    enable_cameras_for_active_players();
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    camera->eyeMode = 1;
    camera->eye.x = 0.0f;
    camera->eye.y = 20.0f;
    camera->eye.z = -64.0f;
    camera->lookAtMode = 1;
    mtxa_from_rotate_z(rand() & 0x7FFF);
    mtxa_tf_vec_xyz(250.0f, 0.0f, 0.0f, &v);
    camera->lookAt.x = v.x;
    camera->lookAt.y = 20.0f + v.y;
    camera->lookAt.z = 100.0f + v.z;
    if (lbl_1_bss_D3354.work->charas[0].ape != NULL)
        lbl_1_bss_D3354.work->charas[0].ape->colorIndex = modeCtrl.currPlayer;
    fade_screen_to_color(0x100, 0, 0x1E);
    g_fade_track_volume(0x44, 0);
}

void fn_1_FDDB4(void)
{
    struct EndingChara *chara;
    struct EndingCamera *camera;
    float t;

    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[0];

    switch (modeCtrl.submodeTimer)
    {
    case 0x19E:
        fn_1_107CF8(modeCtrl.submodeTimer - 0xB4);
        break;
    case 0x134:
        call_SoundReqID_arg_0(0x230);
        break;
    case 0xF0:
        chara->unk4 = 1;
        chara->unk6 = modeCtrl.submodeTimer - 0x78;
        break;
    case 0xB4:
        t = 1.0f / (float)modeCtrl.submodeTimer;
        camera->eyeVel.x = -camera->eye.x * t;
        camera->eyeVel.y = (20.0f - camera->eye.y) * t;
        camera->eyeVel.z = (-61.0f - camera->eye.z) * t;
        break;
    case 0x87:
        SoundReq(lbl_1_data_636CC[chara->ape->charaId]);
        break;
    case 0x78:
        fn_1_107D78();
        call_SoundReqID_arg_0(0x231);
        break;
    case 0x69:
        camera->lookAtMode = 2;
        camera->lookAtChara = 0;
        camera->lookAtBone = 0;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.0f;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_1_bss_D3354.nextState = 3;
}

void fn_1_FDFCC(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    struct EndingWork *work;
    struct TextBox tb;
    Vec v;
    int j;

    lbl_1_bss_D3354.nextState = 4;
    modeCtrl.submodeTimer = 0x3C;
    modeCtrl.unk18 = 0x1E;
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    camera->eyeMode = 1;
    mtxa_from_rotate_z(g_font_char_var_3 << 6);
    mtxa_tf_vec_xyz(0.0f, 0.5f, 0.0f, &v);
    camera->eye.x = v.x;
    camera->eye.y = 20.75f + 0.5f * v.y;
    camera->eye.z = -61.0f + v.z;
    camera->lookAtMode = 2;
    camera->lookAtChara = 0;
    camera->lookAtBone = 0;
    camera->lookAtOffset.x = 0.0f;
    camera->lookAtOffset.y = 0.0f;
    camera->lookAtOffset.z = 0.0f;
    work = lbl_1_bss_D3354.work;
    work->msgLine = 0;
    fn_1_1090E0(&work->msg, work->msgBuf);
    if (work->msg.count > 0)
    {
        SHOW_MSG_LINES(work, tb, j, 8, 0x142);
    }
    chara = &lbl_1_bss_D3354.work->charas[0];
    chara->unkA = 1;
    chara->unk38 = 0;
}

void fn_1_FE25C(void)
{
    struct EndingCamera *camera;
    struct EndingChara *chara;
    Vec v;

    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[0];
    mtxa_from_rotate_z(g_font_char_var_3 << 6);
    mtxa_tf_vec_xyz(0.0f, 0.5f, 0.0f, &v);
    camera->eye.x = v.x;
    camera->eye.y = 20.75f + 0.5f * v.y;
    camera->eye.z = -61.0f + v.z;
    CHARA_TALK(chara);
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (merged_digital_inputs[2] & 0x100))
    {
        struct TextBox tb;
        struct EndingWork *work;
        int j;

        modeCtrl.unk18 = 0x1E;
        work = lbl_1_bss_D3354.work;
        if (work->msg.count != 0)
        {
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
            SHOW_MSG_LINES(work, tb, j, 8, 0x142);
        }
        else
        {
            lbl_1_bss_D3354.nextState = 5;
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void fn_1_FE568(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;

    lbl_1_bss_D3354.nextState = 6;
    modeCtrl.submodeTimer = 0x168;
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    camera->eyeMode = 1;
    camera->eye.x = 0.0f;
    camera->eye.y = 20.0f;
    camera->eye.z = -62.0f;
    camera->lookAtMode = 2;
    camera->lookAtChara = 0;
    camera->lookAtBone = 0;
    camera->lookAtOffset.x = 0.0f;
    camera->lookAtOffset.y = 0.0f;
    camera->lookAtOffset.z = 0.0f;
    lbl_1_bss_D3354.work->charas[0].unkA = 2;
}

void fn_1_FE698(void)
{
    switch (modeCtrl.submodeTimer)
    {
    case 0xF0:
        lbl_1_bss_D3354.work->charas[0].unk4 = 4;
        break;
    case 0x117:
        SoundReq(lbl_1_data_636D4[active_monkey_id[modeCtrl.currPlayer]]);
        break;
    case 0x1E:
        fade_screen_to_color(1, 0xFFFFFF, modeCtrl.submodeTimer);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_1_bss_D3354.nextState = 0x1F;
}

void fn_1_FE7AC(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    int stageId;
    float t;

    lbl_1_bss_D3354.nextState = 8;
    modeCtrl.submodeTimer = 0x186;
    dest_all_events();
    switch (modeCtrl.difficulty)
    {
    case 0:
        stageId = 0xC6;
        break;
    case 1:
        stageId = 0xC6;
        break;
    case 2:
    default:
        stageId = 0xC6;
        break;
    }
    load_stage(stageId);
    event_init(0);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(0xB);
    light_init(current_stage_id);
    g_init_rendefc_for_stage();
    fn_1_1050D8(0, active_monkey_id[modeCtrl.currPlayer]);
    lbl_1_bss_D3354.work->charas[0].unk4 = 6;
    lbl_1_bss_D3354.work->charas[0].unk6 = modeCtrl.submodeTimer - 0xB4;
    enable_cameras_for_active_players();
    RESET_ALL_CAMERAS(cam, curr, i);
    t = 1.0f / (float)(modeCtrl.submodeTimer - 0x12C);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    camera->eyeMode = 1;
    camera->eye.x = -1.0f;
    camera->eye.y = 20.0f;
    camera->eye.z = -1.0f;
    camera->eyeVel.y = 10.0f * t;
    camera->lookAtMode = 1;
    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.0f;
    camera->lookAt.z = 0.0f;
    if (lbl_1_bss_D3354.work->charas[0].ape != NULL)
        lbl_1_bss_D3354.work->charas[0].ape->colorIndex = modeCtrl.currPlayer;
    fade_screen_to_color(0x100, 0, 0x1E);
    g_fade_track_volume(0x44, 0);
    call_SoundReqID_arg_0(0x232);
}

#define ORBIT_ROUND_CHARA(camera, chara, v, t, dist) \
    camera->eyeMode = 1; \
    mtxa_from_rotate_y(rand() & 0x7FFF); \
    mtxa_rotate_x(rand() & 0x7FFF); \
    mtxa_tf_vec_xyz(0.0f, 0.0f, dist, &camera->eye); \
    camera->eye.x += chara->pos.x; \
    camera->eye.y += chara->pos.y; \
    camera->eye.z += chara->pos.z; \
    mtxa_rotate_y((rand() & 0x7FF) - 0x400); \
    mtxa_rotate_x((rand() & 0x7FF) - 0x400); \
    mtxa_tf_vec_xyz(0.0f, 0.0f, dist, &v); \
    camera->eyeVel.x = t * ((v.x + chara->pos.x) - camera->eye.x); \
    camera->eyeVel.y = t * ((v.y + chara->pos.y) - camera->eye.y); \
    camera->eyeVel.z = t * ((v.z + chara->pos.z) - camera->eye.z)

void fn_1_FEA88(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    Vec v;
    float t;
    float dist;

    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x177:
        camera->lookAtMode = 2;
        camera->lookAtChara = 0;
        camera->lookAtBone = 0;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.0f;
        SoundReq(lbl_1_data_636DC[active_monkey_id[modeCtrl.currPlayer]]);
        break;
    case 0x12C:
        camera->eyeMode = 2;
        camera->eyeChara = 0;
        camera->eyeBone = 0;
        camera->eyeOffset.x = 0.0f;
        camera->eyeOffset.y = 0.0f;
        camera->eyeOffset.z = -2.0f;
        break;
    case 0xB4:
        break;
    case 0x43:
        chara->ape->flags &= ~0x3000;
        if (modeCtrl.playerCount > 1)
            chara->unk56 = modeCtrl.currPlayer;
        else
            chara->unk56 = 3;
        fn_1_6869C(0x13);
        RESET_ALL_CAMERAS(cam, curr, i);
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x38);
        dist = 1.25f * (0.9f + 0.2f * (rand() / 32767.0f));
        ORBIT_ROUND_CHARA(camera, chara, v, t, dist);
        break;
    case 0x3C:
        chara->ape->flags &= ~0x3000;
        if (modeCtrl.playerCount > 1)
            chara->unk56 = modeCtrl.currPlayer;
        else
            chara->unk56 = 3;
        fn_1_6869C(0x13);
        RESET_ALL_CAMERAS(cam, curr, i);
        t = 1.0f / (float)modeCtrl.submodeTimer;
        dist = 10.0f * (0.9f + 0.2f * (rand() / 32767.0f));
        ORBIT_ROUND_CHARA(camera, chara, v, t, dist);
        break;
    case 0x69:
    case 0x42:
    case 0x3B:
        chara->unkA = 4;
        break;
    case 0x68:
    case 0x41:
    case 0x3A:
        if (chara->ape->unk2A4 != 4)
            modeCtrl.submodeTimer++;
        else
            chara->ape->flags |= 0x1000;
        break;
    case 0x4B:
    case 0x40:
    case 0x39:
        modeCtrl.submodeTimer++;
        break;
    case 0x4A:
    case 0x3F:
    case 0x38:
        modeCtrl.submodeTimer++;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
    {
        lbl_1_bss_D3354.nextState = 9;
        lbl_1_bss_D3354.flags &= ~8;
    }
}

void fn_1_FF178(void)
{
    struct TextBox tb;
    struct EndingWork *work;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    float t;

    lbl_1_bss_D3354.nextState = 0xA;
    modeCtrl.submodeTimer = 0x3C;
    modeCtrl.unk18 = 0x1E;
    work = lbl_1_bss_D3354.work;
    work->msgLine = 0;
    fn_1_1090E0(&work->msg, work->msgBuf);
    if (work->msg.count > 0)
    {
        SHOW_MSG_LINES(work, tb, i, 0xB, 0xA0);
    }
    t = (float)(modeCtrl.submodeTimer - 0x12C);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    camera->eyeMode = 2;
    camera->eyeChara = 0;
    camera->eyeBone = 5;
    camera->eyeOffset.x = 1.5f;
    camera->eyeOffset.y = 0.0f;
    camera->eyeOffset.z = 0.3f;
    camera->lookAtMode = 2;
    camera->lookAtChara = 0;
    camera->lookAtBone = 5;
    camera->lookAtOffset.x = 0.0f;
    camera->lookAtOffset.y = 0.0f;
    camera->lookAtOffset.z = 0.3f;
    chara = &lbl_1_bss_D3354.work->charas[0];
    chara->unkA = 6;
    chara->ape->flags &= ~0x1000;
    chara->unk38 = 0;
}

void fn_1_FF3BC(void)
{
    struct EndingChara *chara;

    chara = &lbl_1_bss_D3354.work->charas[0];
    CHARA_TALK(chara);
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (merged_digital_inputs[2] & 0x100))
    {
        struct TextBox tb;
        struct EndingWork *work;
        int j;

        modeCtrl.unk18 = 0x1E;
        work = lbl_1_bss_D3354.work;
        if (work->msg.count != 0)
        {
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
            SHOW_MSG_LINES(work, tb, j, 0xB, 0xA0);
        }
        else
        {
            lbl_1_bss_D3354.nextState = 0xB;
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void fn_1_FF650(void)
{
    lbl_1_bss_D3354.nextState = 0xC;
    modeCtrl.submodeTimer = 0x21C;
    lbl_1_bss_D3354.work->charas[0].unkA = 7;
    lbl_1_bss_D3354.work->charas[0].ape->flags &= ~0x4000;
}

void fn_1_FF6AC(void)
{
    struct EndingCamera *camera;
    struct EndingChara *chara;

    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x21B:
        if (chara->ape->unk2A4 != 7)
            modeCtrl.submodeTimer++;
        else
        {
            chara->ape->flags |= 0x1000;
            camera->lookAtMode = 2;
            camera->lookAtChara = 0;
            camera->lookAtBone = 5;
            camera->lookAtOffset.x = 0.0f;
            camera->lookAtOffset.y = 0.0f;
            camera->lookAtOffset.z = 0.0f;
        }
        break;
    case 0x21A:
        modeCtrl.submodeTimer++;
        break;
    case 0x1A4:
        lbl_1_bss_D3354.flags |= 0x20;
        break;
    case 0xF0:
        lbl_1_bss_D3354.flags &= ~0x20;
        break;
    case 0x10E:
        fn_1_107C64(1, modeCtrl.submodeTimer - 0x1E);
        break;
    case 0x1E:
        fade_screen_to_color(1, 0xFFFFFF, modeCtrl.submodeTimer);
        call_SoundReqID_arg_0(0x22F);
        break;
    }
    if (modeCtrl.submodeTimer < 0x1A4 && modeCtrl.submodeTimer > 0x168)
    {
        float t = -0.016666668f * (float)(0x168 - modeCtrl.submodeTimer);

        lbl_1_bss_D3354.work->unkAAA8 = 0.5f + 0.5f * t;
    }
    else if (modeCtrl.submodeTimer < 0x12C && modeCtrl.submodeTimer > 0xF0)
    {
        float t = -0.016666668f * (float)(0xF0 - modeCtrl.submodeTimer);

        lbl_1_bss_D3354.work->unkAAA8 = 0.5f + 0.5f * (1.0f - t);
    }
    if (modeCtrl.submodeTimer < 0x1A4 && modeCtrl.submodeTimer > 0xF0)
    {
        float t = -0.0055555557f * (float)(0xF0 - modeCtrl.submodeTimer);

        lbl_1_bss_D3354.work->unkAAAC = 0.6f + 0.19999999f * t;
    }
    if (modeCtrl.submodeTimer < 0x12C && modeCtrl.submodeTimer > 0)
    {
        float t = -0.0033333334f * (float)(-modeCtrl.submodeTimer);

        t = 1.0f - t;
        if (rand() / 32767.0f < t)
            fn_1_107BD0(1);
        if (g_font_char_var_3 % 90 == 0)
            call_SoundReqID_arg_0(0x22E);
    }
    camera->eyeVel.x *= 0.99f;
    camera->eyeVel.y *= 0.99f;
    camera->eyeVel.z *= 0.99f;
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_1_bss_D3354.nextState = 0x1F;
}

void fn_1_FFB5C(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    int stageId;
    int id;

    lbl_1_bss_D3354.nextState = 0xE;
    modeCtrl.submodeTimer = 0xB4;
    dest_all_events();
    switch (modeCtrl.difficulty)
    {
    case 0:
        stageId = 0xC6;
        break;
    case 1:
        stageId = 0xC6;
        break;
    case 2:
    default:
        stageId = 0xC6;
        break;
    }
    load_stage(stageId);
    event_init(0);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(0xB);
    light_init(current_stage_id);
    g_init_rendefc_for_stage();
    fn_1_1050D8(0, 0);
    fn_1_1050D8(1, 1);
    fn_1_1050D8(2, 2);
    fn_1_1050D8(3, 3);
    lbl_1_bss_D3354.work->charas[0].unk4 = 9;
    lbl_1_bss_D3354.work->charas[1].unk4 = 9;
    lbl_1_bss_D3354.work->charas[2].unk4 = 9;
    enable_cameras_for_active_players();
    RESET_ALL_CAMERAS(cam, curr, i);
    {
        struct EndingCamera *camera;

        camera = &lbl_1_bss_D3354.work->camera;
        memset(camera, 0, sizeof(*camera));
        camera->eyeMode = 1;
        mtxa_from_rotate_y(modeCtrl.submodeTimer << 2);
        mtxa_tf_vec_xyz(0.0f, 20.0f, 10.0f, &camera->eye);
        camera->lookAtMode = 1;
        camera->lookAt.x = 0.0f;
        camera->lookAt.y = 0.0f;
        camera->lookAt.z = 0.0f;
    }
    id = active_monkey_id[modeCtrl.currPlayer];
    if (id == 3)
    {
        struct EndingChara *chara;

        chara = &lbl_1_bss_D3354.work->charas[id];
        chara->ape->flags &= ~0x20;
        chara->unkA = 0x12;
        chara->pos.x = -20.38f;
        chara->pos.y = -2.34f;
        chara->pos.z = -20.39f;
        if (modeCtrl.playerCount == 1)
            chara->unk56 = 3;
        else
            chara->unk56 = (s16)modeCtrl.currPlayer;
        chara->unk58 = (s16)(rand() & 0x7FFF);
        chara->unk5A = (s16)(rand() & 0x7FFF);
        chara->unk5C = (s16)(rand() & 0x7FFF);
    }
    if (lbl_1_bss_D3354.work->charas[modeCtrl.currPlayer].ape != NULL)
        lbl_1_bss_D3354.work->charas[modeCtrl.currPlayer].ape->colorIndex = modeCtrl.currPlayer;
    fade_screen_to_color(0x100, 0, 0x1E);
    g_fade_track_volume(0x44, 0);
}

void fn_1_FFF04(void)
{
    struct EndingCamera *camera;

    camera = &lbl_1_bss_D3354.work->camera;
    mtxa_from_rotate_y(modeCtrl.submodeTimer << 2);
    mtxa_tf_vec_xyz(0.0f, 20.0f, 12.0f, &camera->eye);
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_1_bss_D3354.nextState = 0xF;
}

void fn_1_FFFCC(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    float t;
    struct TextBox tb;
    struct EndingWork *work;
    int j;

    lbl_1_bss_D3354.nextState = 0x10;
    modeCtrl.submodeTimer = 0x12C;
    modeCtrl.unk18 = 0x1E;
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]];
    t = (float)modeCtrl.submodeTimer / 300.0f;
    mtxa_from_translate(&chara->pos);
    mtxa_rotate_y(chara->rotY);
    mtxa_rotate_x(chara->rotX);
    mtxa_rotate_z(chara->rotZ);
    camera->eyeMode = 1;
    mtxa_tf_point_xyz(-4.0f * (t - 0.5f), -0.2f, -2.0f, &camera->eye);
    camera->eyeVel.x = 0.0f;
    camera->eyeVel.y = 0.0f;
    camera->eyeVel.z = 0.0f;
    camera->lookAtMode = 1;
    mtxa_tf_point_xyz(0.0f, 0.2f, 0.0f, &camera->lookAt);
    camera->lookAtVel.x = 0.0f;
    camera->lookAtVel.y = 0.0f;
    camera->lookAtVel.z = 0.0f;
    lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]].unk70 = 1;
    work = lbl_1_bss_D3354.work;
    work->msgLine = 0;
    fn_1_1090E0(&work->msg, work->msgBuf);
    if (work->msg.count > 0)
    {
        SHOW_MSG_LINES(work, tb, j, 8, 0x142);
    }
    {
        struct EndingChara *c;

        c = &lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]];
        c->unk38 = 0;
        if (active_monkey_id[modeCtrl.currPlayer] != 3)
            c->unkA = 0xB;
        else
            c->unkA = 0xC;
    }
    lbl_1_bss_D3354.work->charas[0].unk36 |= 1;
    lbl_1_bss_D3354.work->charas[1].unk36 |= 1;
    lbl_1_bss_D3354.work->charas[2].unk36 |= 1;
    lbl_1_bss_D3354.work->charas[3].unk36 |= 1;
}

void fn_1_1003EC(void)
{
    struct EndingCamera *camera;
    struct EndingChara *chara;
    float t;
    struct TextBox tb;

    camera = &lbl_1_bss_D3354.work->camera;
    chara = &lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]];
    t = (float)modeCtrl.submodeTimer / 300.0f;
    mtxa_from_translate(&chara->pos);
    mtxa_rotate_y(chara->rotY);
    mtxa_rotate_x(chara->rotX);
    mtxa_rotate_z(chara->rotZ);
    mtxa_tf_point_xyz(-4.0f * (t - 0.5f), -0.2f, -2.0f, &camera->eye);
    CHARA_TALK(chara);
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (merged_digital_inputs[2] & 0x100))
    {
        struct EndingWork *work;
        int j;

        modeCtrl.unk18 = 0x1E;
        work = lbl_1_bss_D3354.work;
        if (work->msg.count != 0)
        {
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
            SHOW_MSG_LINES(work, tb, j, 8, 0x142);
        }
        else
        {
            lbl_1_bss_D3354.nextState = 0x11;
            g_display_playpoint_or_gift_message_child(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void fn_1_100744(void)
{
    lbl_1_bss_D3354.nextState = 0x12;
    modeCtrl.submodeTimer = 0x1E0;
    fn_1_103D8C(modeCtrl.submodeTimer - 0x1A4);
    lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]].unk74 = cameras[0].eye;
}

void fn_1_1007EC(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    Vec v;
    float t2;

    switch (modeCtrl.submodeTimer)
    {
    case 0x1DF:
    {
        float t;

        RESET_ALL_CAMERAS(cam, curr, i);
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x168);
        camera->eyeMode = 1;
        camera->eye.x = 0.0f;
        camera->eye.y = 0.0f;
        camera->eye.z = 6.5f;
        camera->eyeVel.x = -camera->eye.x * t;
        camera->eyeVel.y = (1.5f - camera->eye.y) * t;
        camera->eyeVel.z = (6.0f - camera->eye.z) * t;
        camera->lookAtMode = 1;
        camera->lookAt = lbl_1_bss_D3354.work->unkAA04.unk8;
        camera->lookAtVel.x = 0.0f;
        camera->lookAtVel.y = 0.0f;
        camera->lookAtVel.z = 0.0f;
        break;
    }
    case 0x1A4:
        lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer]].ape->flags &= ~0x40;
        call_SoundReqID_arg_0(0x234);
        break;
    case 0x168:
    {
        float t;

        lbl_1_bss_D3354.work->charas[0].unkA = 8;
        lbl_1_bss_D3354.work->charas[1].unkA = 8;
        lbl_1_bss_D3354.work->charas[2].unkA = 8;
        SoundReq(lbl_1_data_636F4[active_monkey_id[modeCtrl.currPlayer] % 3]);
        RESET_ALL_CAMERAS(cam, curr, i);
        camera = &lbl_1_bss_D3354.work->camera;
        chara = &lbl_1_bss_D3354.work->charas[active_monkey_id[modeCtrl.currPlayer] % 3];
        memset(camera, 0, sizeof(*camera));
        mtxa_from_translate(&chara->pos);
        mtxa_rotate_y(chara->rotY);
        mtxa_rotate_x(chara->rotX);
        mtxa_rotate_z(chara->rotZ);
        camera->eyeMode = 1;
        mtxa_tf_point_xyz(1.0f, 3.0f, -4.0f, &camera->eye);
        t = 1.0f / (float)modeCtrl.submodeTimer;
        camera->eyeVel.y = (8.0f - camera->eye.y) * t;
        mtxa_tf_point_xyz(0.0f, 0.2f, 0.0f, &camera->lookAt);
        break;
    }
    case 0x12C:
    {
        float t;

        fn_1_107C64(1, modeCtrl.submodeTimer - 0x1E);
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)modeCtrl.submodeTimer;
        camera->eyeVel.x = camera->eye.x * t;
        camera->eyeVel.z = camera->eye.z * t;
        camera->lookAtVel.x = -camera->lookAt.x * t;
        camera->lookAtVel.y = (40.0f - camera->lookAt.y) * t;
        camera->lookAtVel.z = -camera->lookAt.z * t;
        break;
    }
    case 0x78:
        lbl_1_bss_D3354.work->charas[0].unk4 = 0x10;
        lbl_1_bss_D3354.work->charas[1].unk4 = 0x10;
        lbl_1_bss_D3354.work->charas[2].unk4 = 0x10;
        lbl_1_bss_D3354.work->charas[3].unk56 = -1;
        v.x = 0.0f;
        v.y = 4.0f;
        v.z = 0.0f;
        lbl_1_bss_D3354.work->charas[0].unk70 = 2;
        lbl_1_bss_D3354.work->charas[1].unk70 = 2;
        lbl_1_bss_D3354.work->charas[2].unk70 = 2;
        lbl_1_bss_D3354.work->charas[0].unk74 = v;
        lbl_1_bss_D3354.work->charas[1].unk74 = v;
        lbl_1_bss_D3354.work->charas[2].unk74 = v;
        lbl_1_bss_D3354.work->charas[0].unkA = 0xC;
        lbl_1_bss_D3354.work->charas[1].unkA = 0xC;
        lbl_1_bss_D3354.work->charas[2].unkA = 0xC;
        break;
    case 0x1E:
        fade_screen_to_color(1, 0xFFFFFF, modeCtrl.submodeTimer);
        call_SoundReqID_arg_0(0x22F);
        break;
    }
    if (modeCtrl.submodeTimer < 0x1DF && modeCtrl.submodeTimer > 0x1A4)
    {
        camera = &lbl_1_bss_D3354.work->camera;
        camera->lookAt = lbl_1_bss_D3354.work->unkAA04.unk8;
    }
    if (modeCtrl.submodeTimer < 0x1A4 && (float)modeCtrl.submodeTimer > 0.0f)
    {
        t2 = -0.0023809525f * -(float)modeCtrl.submodeTimer;
        t2 = 1.0f - t2;
        if (rand() / 32767.0f < t2)
            fn_1_107BD0(1);
        if (g_font_char_var_3 % 90 == 0)
            call_SoundReqID_arg_0(0x22E);
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_1_bss_D3354.nextState = 0x13;
}

void fn_1_101010(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    float t;

    lbl_1_bss_D3354.nextState = 0x14;
    modeCtrl.submodeTimer = 0x168;
    fade_screen_to_color(0, 0, 0x1E);
    fn_1_1053B0();
    fn_1_107864();
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    memset(camera, 0, sizeof(*camera));
    t = 1.0f / (float)(modeCtrl.submodeTimer - 0xF0);
    camera->eyeMode = 1;
    camera->eye.x = 0.0f;
    camera->eye.y = 0.5f;
    camera->eye.z = 5.0f;
    camera->eyeVel.x = 2.0f * -camera->eye.x * t;
    camera->eyeVel.y = (2.1f - camera->eye.y) * t;
    camera->eyeVel.z = (2.0f - camera->eye.z) * t;
    camera->lookAtMode = 1;
    camera->lookAt = lbl_1_data_635B4[active_monkey_id[modeCtrl.currPlayer] % 3];
    camera->lookAt.y = 0.0f;
    camera->lookAtVel.x = 2.0f * -camera->lookAt.x * t;
    camera->lookAtVel.y = (0.4f - camera->lookAt.y) * t;
    camera->lookAtVel.z = (6.0f - camera->lookAt.z) * t;
    lbl_1_bss_D3354.work->charas[0].unk38 = 1.0f + 120.0f * (rand() / 32767.0f);
    lbl_1_bss_D3354.work->charas[1].unk38 = 1.0f + 120.0f * (rand() / 32767.0f);
    lbl_1_bss_D3354.work->charas[2].unk38 = 1.0f + 120.0f * (rand() / 32767.0f);
}

void fn_1_101340(void)
{
    int i;
    struct EndingChara *chara;

    switch (modeCtrl.submodeTimer)
    {
    case 0xF0:
    {
        struct Camera *cam;
        struct Camera *curr;
        struct EndingCamera *camera;
        float t;
        int j;

        RESET_ALL_CAMERAS(cam, curr, j);
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)modeCtrl.submodeTimer;
        camera->eyeMode = 1;
        camera->eye.x = -0.4f;
        camera->eye.y = -0.35f;
        camera->eye.z = 4.0f;
        camera->eyeVel.x = (-0.6f - camera->eye.x) * t;
        camera->eyeVel.y = (-0.3f - camera->eye.y) * t;
        camera->eyeVel.z = (14.8f - camera->eye.z) * t;
        camera->lookAtMode = 1;
        camera->lookAt.x = 0.0f;
        camera->lookAt.y = 2.0f;
        camera->lookAt.z = 2.0f;
        camera->lookAtVel.x = -camera->lookAt.x * t;
        camera->lookAtVel.y = (2.5f - camera->lookAt.y) * t;
        camera->lookAtVel.z = (2.0f - camera->lookAt.z) * t;
        break;
    }
    case 0:
        break;
    }
    chara = &lbl_1_bss_D3354.work->charas[0];
    for (i = 3; i > 0; i--, chara++)
    {
        if (chara->unk38 != 0)
        {
            chara->unk38--;
            if (chara->unk38 == 0)
                SoundReq(lbl_1_data_636FC[chara->ape->charaId]);
        }
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_1_bss_D3354.nextState = 0x15;
}

void fn_1_101590(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    float t;

    lbl_1_bss_D3354.nextState = 0x16;
    modeCtrl.submodeTimer = 0x2D0;
    lbl_1_bss_D3354.work->charas[0].ape->flags |= 0x20;
    lbl_1_bss_D3354.work->charas[1].ape->flags |= 0x20;
    lbl_1_bss_D3354.work->charas[2].ape->flags |= 0x20;
    lbl_1_bss_D3354.work->charas[3].ape->flags |= 0x20;
    RESET_ALL_CAMERAS(cam, curr, i);
    camera = &lbl_1_bss_D3354.work->camera;
    t = 0.008333334f;
    camera->eyeMode = 1;
    camera->eye.x = 0.0f;
    camera->eye.y = 0.2f;
    camera->eye.z = 5.0f;
    camera->eyeVel.x = -camera->eye.x * t;
    camera->eyeVel.y = (0.2f - camera->eye.y) * t;
    camera->eyeVel.z = (4.5f - camera->eye.z) * t;
    camera->lookAtMode = 1;
    camera->lookAt.x = 0.0f;
    camera->lookAt.y = 0.2f;
    camera->lookAt.z = 3.5f;
    camera->lookAtVel.x = -camera->lookAt.x * t;
    camera->lookAtVel.y = (0.25f - camera->lookAt.y) * t;
    camera->lookAtVel.z = (3.5f - camera->lookAt.z) * t;
}

void fn_1_101754(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    float t;

    switch (modeCtrl.submodeTimer)
    {
    case 0x2B2:
        lbl_1_bss_D3354.flags |= 4;
        call_SoundReqID_arg_0(0x235);
        break;
    case 0x258:
        lbl_1_bss_D3354.work->charas[0].unk4 = 0x11;
        lbl_1_bss_D3354.work->charas[1].unk4 = 0x11;
        lbl_1_bss_D3354.work->charas[2].unk4 = 0x11;
        break;
    case 0x257:
        lbl_1_bss_D3354.work->charas[0].ape->flags &= ~0x20;
        lbl_1_bss_D3354.work->charas[1].ape->flags &= ~0x20;
        lbl_1_bss_D3354.work->charas[2].ape->flags &= ~0x20;
        RESET_ALL_CAMERAS(cam, curr, i);
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0xD8);
        camera->eyeMode = 1;
        camera->eye.x = -2.0f;
        camera->eye.y = 0.3f;
        camera->eye.z = 5.0f;
        camera->eyeVel.x = (-2.5f - camera->eye.x) * t;
        camera->eyeVel.y = (0.4f - camera->eye.y) * t;
        camera->eyeVel.z = (4.5f - camera->eye.z) * t;
        camera->lookAtMode = 2;
        camera->lookAtChara = (s16)(active_monkey_id[modeCtrl.currPlayer] % 3);
        camera->lookAtBone = 0;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.0f;
        break;
    case 0x1E0:
        lbl_1_bss_D3354.work->charas[3].ape->flags &= ~0x20;
        lbl_1_bss_D3354.work->charas[3].unk4 = 0x13;
        break;
    case 0xD8:
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x3C);
        camera->eyeMode = 1;
        camera->eyeVel.x = (2.0f - camera->eye.x) * t;
        camera->eyeVel.y = (0.3f - camera->eye.y) * t;
        camera->eyeVel.z = (3.5f - camera->eye.z) * t;
        camera->lookAtMode = 2;
        camera->lookAtChara = 3;
        camera->lookAtBone = 0;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.0f;
        SoundReq(0x19A);
        break;
    case 0x8A:
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x3C);
        camera->eyeMode = 1;
        camera->eyeVel.x = (2.0f - camera->eye.x) * t;
        camera->eyeVel.y = (0.3f - camera->eye.y) * t;
        camera->eyeVel.z = (4.5f - camera->eye.z) * t;
        camera->lookAtMode = 2;
        camera->lookAtChara = 3;
        camera->lookAtBone = 0;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.0f;
        break;
    case 0x3C:
        lbl_1_bss_D3354.flags &= ~4;
        call_SoundReqID_arg_0(0x236);
        camera = &lbl_1_bss_D3354.work->camera;
        camera->eyeMode = 1;
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_1_bss_D3354.nextState = 0x17;
}

void fn_1_101C64(void)
{
    struct EndingCamera *camera;

    lbl_1_bss_D3354.nextState = 0x18;
    modeCtrl.submodeTimer = 0x12C;
    lbl_1_bss_D3354.work->charas[0].unk4 = 0x15;
    lbl_1_bss_D3354.work->charas[1].unk4 = 0x15;
    lbl_1_bss_D3354.work->charas[2].unk4 = 0x15;
    lbl_1_bss_D3354.work->charas[0].unk70 = 0;
    lbl_1_bss_D3354.work->charas[1].unk70 = 0;
    lbl_1_bss_D3354.work->charas[2].unk70 = 0;
    camera = &lbl_1_bss_D3354.work->camera;
    camera->eyeMode = 1;
    camera->eye.x = 2.0f;
    camera->eye.y = 0.3f;
    camera->eye.z = 4.5f;
    camera->eyeVel.x = 0.0f;
    camera->eyeVel.y = 0.0f;
    camera->eyeVel.z = 0.0f;
    camera->lookAtMode = 2;
    camera->lookAtChara = 3;
    camera->lookAtBone = 0;
    camera->lookAtOffset.x = 0.0f;
    camera->lookAtOffset.y = 0.0f;
    camera->lookAtOffset.z = 0.0f;
    call_SoundReqID_arg_0(0x238);
    SoundReq(0x17C);
    lbl_1_bss_D3354.work->charas[0].unk36 &= ~1;
    lbl_1_bss_D3354.work->charas[1].unk36 &= ~1;
    lbl_1_bss_D3354.work->charas[2].unk36 &= ~1;
}

void fn_1_101E24(void)
{
    struct EndingChara *chara;
    int frame;

    chara = &lbl_1_bss_D3354.work->charas[3];
    frame = 0;
    if (chara->ape->unk2A4 == 8 && frame == 0x2A)
        SoundReq(0x182);
    if (chara->ape->unk2A4 == 0xA)
    {
        if (frame == 1)
            SoundReq(0x186);
        if (frame == 0x14 || frame == 0x24 || frame == 0x34 || frame == 0x44)
            call_SoundReqID_arg_0(0x238);
    }
    if (chara->ape->unk2A4 == 0xB && frame == 0xB2)
        SoundReq(0x19A);
    if (chara->ape->unk2A4 == 0xE && (frame == 0x10 || frame == 0x2E))
        SoundReq(0x187);
    if (chara->unk8 == 0xB && frame > 0xC3)
    {
        fn_1_107A18();
        lbl_1_bss_D3354.nextState = 0x19;
        lbl_1_bss_D3354.flags |= 2;
        lbl_1_bss_D3354.work->timer = 0.0f;
        call_SoundReqID_arg_0(0x238);
    }
}

void fn_1_101F78(void)
{
    lbl_1_bss_D3354.nextState = 0x1A;
    modeCtrl.submodeTimer = 0x1E0;
    modeCtrl.unk18 = 0;
    lbl_1_bss_D3354.work->unkAA04.unk2 = 4;
}

void fn_1_101FC4(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    float t;

    camera = &lbl_1_bss_D3354.work->camera;
    switch (modeCtrl.submodeTimer)
    {
    case 0x1D1:
        RESET_ALL_CAMERAS(cam, curr, i);
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x12C);
        camera->eyeMode = 0;
        camera->eye.x = 0.0f;
        camera->eye.y = -0.34f;
        camera->eye.z = 5.0f;
        camera->eyeVel.x = -camera->eye.x * t;
        camera->eyeVel.y = (-0.25f - camera->eye.y) * t;
        camera->eyeVel.z = (7.8f - camera->eye.z) * t;
        camera->lookAtMode = 2;
        camera->lookAtChara = 3;
        camera->lookAtBone = 5;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.6f;
        break;
    case 0x195:
        camera->eyeMode = 1;
        break;
    case 0x186:
        fn_1_107A88();
        break;
    case 0x12C:
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    case 0xD2:
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x96);
        camera->eyeMode = 1;
        camera->eyeVel.x = -camera->eye.x * t;
        camera->eyeVel.y = (0.7f - camera->eye.y) * t;
        camera->eyeVel.z = (4.2f - camera->eye.z) * t;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x5A);
        camera->lookAtMode = 1;
        camera->lookAt = cameras[modeCtrl.currPlayer].lookAt;
        camera->lookAtVel.x = -camera->lookAt.x * t;
        camera->lookAtVel.y = (0.1f - camera->lookAt.y) * t;
        camera->lookAtVel.z = -camera->lookAt.z * t;
        break;
    case 0x96:
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x5A);
        camera->eyeVel.x = -camera->eye.x * t;
        camera->eyeVel.y = (-0.4f - camera->eye.y) * t;
        camera->eyeVel.z = (2.6f - camera->eye.z) * t;
        SoundReq(0xEF);
        SoundReq(0x103);
        SoundReq(0x16E);
        break;
    case 0x5A:
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        camera->lookAtVel.x = 0.0f;
        camera->lookAtVel.y = 0.0f;
        camera->lookAtVel.z = 0.0f;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
    {
        if (active_monkey_id[modeCtrl.currPlayer] != 3)
            lbl_1_bss_D3354.nextState = 0x1B;
        else
            lbl_1_bss_D3354.nextState = 0x1D;
    }
}

void fn_1_102400(void)
{
    lbl_1_bss_D3354.nextState = 0x1E;
    modeCtrl.submodeTimer = 0x14A;
    modeCtrl.unk18 = 0;
}

void fn_1_102434(void)
{
    struct EndingChara *chara;
    int id;
    struct EndingCamera *camera;
    Vec v;
    float t;

    camera = &lbl_1_bss_D3354.work->camera;
    switch (modeCtrl.submodeTimer)
    {
    case 0x14A:
    {
        struct Camera *cam;
        struct Camera *curr;
        int i;

        RESET_ALL_CAMERAS(cam, curr, i);
    }
        camera = &lbl_1_bss_D3354.work->camera;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0xD2);
        id = active_monkey_id[modeCtrl.currPlayer];
        chara = &lbl_1_bss_D3354.work->charas[id];
        camera->eyeMode = 1;
        camera->eye.x = chara->pos.x;
        camera->eye.y = chara->pos.y - 0.5f;
        camera->eye.z = chara->pos.z - 2.0f;
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.15f * t;
        camera->eyeVel.z = t;
        camera->lookAtMode = 2;
        camera->lookAtChara = (s16)id;
        camera->lookAtBone = 5;
        camera->lookAtOffset.x = 0.0f;
        camera->lookAtOffset.y = 0.0f;
        camera->lookAtOffset.z = 0.6f;
        SoundReq(0x19A);
        break;
    case 0xD2:
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    case 0xB4:
        id = active_monkey_id[modeCtrl.currPlayer];
        lbl_1_bss_D3354.work->charas[3].unkA = 0x11;
        chara = &lbl_1_bss_D3354.work->charas[id];
        {
            struct Camera *cam;
            struct Camera *curr;
            int i;

            RESET_ALL_CAMERAS(cam, curr, i);
        }
        camera->eyeMode = 1;
        camera->eye.x = chara->pos.x;
        camera->eye.y = 0.5f + chara->pos.y;
        camera->eye.z = 2.5f + chara->pos.z;
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        camera->lookAtMode = 1;
        camera->lookAt.x = chara->pos.x;
        camera->lookAt.y = 0.25f + chara->pos.y;
        camera->lookAt.z = chara->pos.z;
        camera->lookAtVel.x = 0.0f;
        camera->lookAtVel.y = 0.0f;
        camera->lookAtVel.z = 0.0f;
        break;
    case 0x42:
        SoundReq(0x189);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0 && modeCtrl.unk18 == 0)
    {
        modeCtrl.unk18 = 0x78;
        fade_screen_to_color(1, 0xFFFFFF, modeCtrl.unk18);
        t = 1.0f / (float)modeCtrl.unk18;
        camera->eyeMode = 1;
        camera->eye = cameras[modeCtrl.currPlayer].eye;
        v.x = camera->eye.x;
        v.y = 0.0f;
        v.z = camera->eye.z;
        vec_set_len(8.0f, &v, &v);
        camera->eyeVel.x = t * (v.x - camera->eye.y);
        camera->eyeVel.y = (4.5f - camera->eye.y) * t;
        camera->eyeVel.z = t * (v.z - camera->eye.y);
    }
    if (modeCtrl.unk18 != 0)
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
            lbl_1_bss_D3354.nextState = 0x1F;
    }
}

void fn_1_102914(void)
{
    lbl_1_bss_D3354.nextState = 0x1C;
    modeCtrl.submodeTimer = 0x276;
    modeCtrl.unk18 = 0;
}

#define ORBIT_CHARA(camera, chara, id, v, t, idExpr, endTime) \
    RESET_ALL_CAMERAS(cam, curr, i); \
    camera = &lbl_1_bss_D3354.work->camera; \
    t = 1.0f / (float)(modeCtrl.submodeTimer - endTime); \
    id = idExpr; \
    chara = &lbl_1_bss_D3354.work->charas[id]; \
    camera->eyeMode = 1; \
    v.x = chara->pos.x; \
    v.y = 0.0f; \
    v.z = chara->pos.z; \
    vec_set_len(1.25f, &v, &v); \
    camera->eye.x = chara->pos.x + v.x; \
    camera->eye.y = chara->pos.y; \
    camera->eye.z = chara->pos.z + v.z; \
    camera->eyeVel.x = -0.5f * v.x * t; \
    camera->eyeVel.y = 0.5f * t; \
    camera->eyeVel.z = t * ((1.0f + chara->pos.z) - camera->eye.z); \
    camera->lookAtMode = 2; \
    camera->lookAtChara = (s16)id; \
    camera->lookAtBone = 0; \
    camera->lookAtOffset.x = 0.0f; \
    camera->lookAtOffset.y = 0.0f; \
    camera->lookAtOffset.z = 0.0f

void fn_1_102948(void)
{
    struct Camera *cam;
    struct Camera *curr;
    int i;
    struct EndingCamera *camera;
    struct EndingChara *chara;
    int id;
    Mtx m;
    Vec v;
    float t;

    camera = &lbl_1_bss_D3354.work->camera;
    switch (modeCtrl.submodeTimer)
    {
    case 0x276:
        lbl_1_bss_D3354.flags |= 0x10;
        ORBIT_CHARA(camera, chara, id, v, t, (active_monkey_id[modeCtrl.currPlayer] + 2) % 3, 0x1FE);
        break;
    case 0x1E0:
        ORBIT_CHARA(camera, chara, id, v, t, (active_monkey_id[modeCtrl.currPlayer] + 1) % 3, 0x168);
        break;
    case 0x14A:
        ORBIT_CHARA(camera, chara, id, v, t, active_monkey_id[modeCtrl.currPlayer], 0xD2);
        break;
    case 0x1FE:
    case 0x168:
    case 0xD2:
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    case 0xB4:
    {
        struct Ape *ape;

        id = active_monkey_id[modeCtrl.currPlayer];
        lbl_1_bss_D3354.work->charas[id].unkA = 0x10;
        chara = &lbl_1_bss_D3354.work->charas[id];
        camera->eyeMode = 1;
        ape = chara->ape;
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(m);
        mtxa_from_translate(&ape->pos);
        mtxa_scale_s(ape->scale);
        mtxa_translate(&ape->unk264);
        mtxa_mult_right(m);
        mtxa_tf_point_xyz(0.0f, 0.0f, 0.3f, &v);
        v.y += -0.3f;
        v.z += 1.0f;
        t = 1.0f / (float)(modeCtrl.submodeTimer - 0x42);
        camera->eyeVel.x = t * (v.x - camera->eye.x);
        camera->eyeVel.y = t * (v.y - camera->eye.y);
        camera->eyeVel.z = t * (v.z - camera->eye.z);
        break;
    }
    case 0x42:
        SoundReq(lbl_1_data_63704[active_monkey_id[modeCtrl.currPlayer]]);
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0 && modeCtrl.unk18 == 0)
    {
        modeCtrl.unk18 = 0x78;
        fade_screen_to_color(1, 0xFFFFFF, modeCtrl.unk18);
    }
    if (modeCtrl.unk18 != 0)
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
            lbl_1_bss_D3354.nextState = 0x1F;
    }
}

void fn_1_1031C0(void)
{
    lbl_1_bss_D3354.nextState = 0x20;
}

void fn_1_1031D4(void) {}

void fn_1_1031D8(void)
{
    struct EndingUnkAA04 *p;

    p = &lbl_1_bss_D3354.work->unkAA04;
    p->unk0 = 0;
}

static inline void jolt_unk44(struct EndingUnkAA04 *q)
{
    q->unk44.x += 0.1f * (rand() / 32767.0f - 0.5f);
    q->unk44.y += 0.1f * (rand() / 32767.0f - 0.5f);
    q->unk44.z += 0.1f * (rand() / 32767.0f - 0.5f);
}

void fn_1_1031F8(void)
{
    struct EndingUnkAA04 *p;

    p = &lbl_1_bss_D3354.work->unkAA04;
    if (p->unk0 != 0)
    {
    switch (p->unk2)
    {
    case 1:
    {
        float t;

        p->unk2 = 2;
        p->unk14.x = 0.0f;
        p->unk14.y = -0.1f;
        p->unk14.z = 0.0f;
        t = p->unk4;
        p->unk8.x = 0.0f;
        p->unk8.y = -0.5f * (t * (2.0f * p->unk14.y + -0.002f * t));
        p->unk8.z = 0.0f;
    }
    case 2:
        p->unk14.y += -0.002f;
        p->unk8.y += p->unk14.y;
        if (p->unk8.y <= 0.0f)
        {
            p->unk8.y = 0.0f;
            p->unk14.y = 0.0f;
            p->unk2 = 3;
            jolt_unk44(p);
        }
        p->unk20.x = 0.4f + p->unk8.x;
        p->unk20.y = p->unk8.y;
        p->unk20.z = 2.95f + p->unk8.z;
        break;
    case 3:
        if (lbl_1_bss_D3354.flags & 4)
        {
            p->unk40 = (s16)((-0x4000 - p->unk3A) >> 7);
            p->unk3A += p->unk40;
            break;
        }
        p->unk40 += (s16)(-0.05f * p->unk3A);
        p->unk3A += p->unk40;
        if (p->unk3A > 0)
        {
            p->unk3A = 0;
            if (p->unk40 > 0)
            {
                if (p->unk40 > 0x800)
                    jolt_unk44(p);
                p->unk40 = -0.4f * p->unk40;
            }
        }
        break;
    case 4:
        p->unk2 = 5;
        p->unk4 = 0x3C;
        p->unk3E += 768.0f * (rand() / 32767.0f - 0.5f);
        p->unk40 += 768.0f * (rand() / 32767.0f - 0.5f);
        p->unk42 += 2304.0f * (rand() / 32767.0f - 0.5f);
        jolt_unk44(p);
    case 5:
        p->unk3E = 0.8f * p->unk3E;
        p->unk40 = 0.8f * p->unk3E;
        p->unk42 = 0.9f * p->unk3E;
        p->unk38 += p->unk3E;
        p->unk3A += p->unk40;
        p->unk3C += p->unk42;
        p->unk4--;
        if (p->unk4 < 0)
        {
            p->unk2 = 6;
            p->unk3E += 2304.0f * (rand() / 32767.0f - 0.5f);
            p->unk40 += -2304.0f * -(rand() / 32767.0f);
            p->unk42 += 2304.0f * (rand() / 32767.0f - 0.5f);
            jolt_unk44(p);
        }
        break;
    case 6:
        p->unk2C.x *= 0.98f;
        p->unk2C.y *= 0.98f;
        p->unk2C.z *= 0.98f;
        p->unk2C.y += -0.001f;
        p->unk20.x += p->unk2C.x;
        p->unk20.y += p->unk2C.y;
        p->unk20.z += p->unk2C.z;
        p->unk3E = 0.95f * p->unk3E;
        p->unk40 = 0.95f * p->unk3E;
        p->unk42 = 0.95f * p->unk3E;
        p->unk38 += p->unk3E;
        p->unk3A += p->unk40;
        p->unk3C += p->unk42;
        break;
    }
    p->unk44.x *= -0.85f;
    p->unk44.y *= -0.85f;
    p->unk44.z *= -0.85f;
    }
}

void fn_1_103CA8(void) {}

void fn_1_103CAC(void)
{
    struct EndingUnkAA04 *p;
    struct GMAModel *model;

    p = &lbl_1_bss_D3354.work->unkAA04;
    if (p->unk0 != 0)
    {
        model = g_bg_gma->modelEntries[6].model;
        mtxa_from_mtxb_translate(&p->unk8);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(model);
        model = g_bg_gma->modelEntries[0x1E].model;
        mtxa_from_mtxb_translate(&p->unk20);
        mtxa_translate(&p->unk44);
        mtxa_rotate_y(p->unk3A);
        mtxa_rotate_x(p->unk38);
        mtxa_rotate_z(p->unk3C);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(model);
    }
}

void fn_1_103D8C(int a)
{
    struct EndingUnkAA04 *p;

    p = &lbl_1_bss_D3354.work->unkAA04;
    memset(p, 0, sizeof(*p));
    p->unk0 = 1;
    p->unk2 = 1;
    p->unk4 = (s16)a;
}

void fn_1_103DF8(void)
{
    if (lbl_1_bss_D3354.work != NULL)
    {
        int i;
        struct EndingChara *chara;

        chara = lbl_1_bss_D3354.work->charas;
        for (i = 0; i < 4; i++, chara++)
            chara->unk56 = -1;
    }
}

#define CHARA_SPAWN_TRAIL(eff, chara, player) \
    do \
    { \
        memset(&eff, 0, sizeof(eff)); \
        eff.type = 4; \
        eff.playerId = (s16)(player); \
        eff.pos = (chara)->pos; \
        spawn_effect(&eff); \
    } while (0)

#define CHARA_INTEGRATE(chara) \
    do \
    { \
        (chara)->pos.x += (chara)->vel.x; \
        (chara)->pos.y += (chara)->vel.y; \
        (chara)->pos.z += (chara)->vel.z; \
    } while (0)

#define CHARA_DAMP(chara, k) \
    do \
    { \
        (chara)->vel.x *= (k); \
        (chara)->vel.y *= (k); \
        (chara)->vel.z *= (k); \
    } while (0)

#define CHARA_RAND_ROT(chara) \
    do \
    { \
        (chara)->unk58 = (s16)(rand() & 0x7FFF); \
        (chara)->unk5A = (s16)(rand() & 0x7FFF); \
        (chara)->unk5C = (s16)(rand() & 0x7FFF); \
    } while (0)

#define CHARA_SNAP_DOWN(chara, hit) \
    do \
    { \
        if (raycast_stage_down(&(chara)->pos, &hit, NULL) != 0) \
        { \
            (chara)->pos = hit.pos; \
            (chara)->pos.y += 0.4f; \
        } \
    } while (0)

void fn_1_103E44(void)
{
    float _pool_base = 0.0f;
    int player;
    int i;
    struct Effect eff;
    struct EndingCharaStart start;
    struct RaycastHit hit;
    struct EndingChara *chara;
    float t;

    player = modeCtrl.currPlayer;
    chara = lbl_1_bss_D3354.work->charas;
    for (i = 0; i < 4; i++, chara++)
    {
        if (chara->ape == NULL)
            continue;
        switch (chara->unk4)
        {
        case 1:
            chara->unk4 = 2;
            chara->ape->flags &= ~0x20;
            chara->unkA = 0;
            chara->rotX = 0;
            chara->rotY = 0;
            chara->rotZ = 0;
            t = chara->unk6;
            chara->pos.x = 0.0f;
            chara->pos.y = 20.0f - 0.5f * (-0.004f * (t * t));
            chara->pos.z = -60.0f;
            chara->vel.x = 0.0f;
            chara->vel.y = 0.0f;
            chara->vel.z = 0.0f;
            break;
        case 2:
            chara->vel.y += -0.004f;
            chara->pos.y += chara->vel.y;
            chara->unk6--;
            if (chara->unk6 <= 0)
            {
                chara->unk4 = 3;
                chara->pos.x = 0.0f;
                chara->pos.y = 20.0f;
                chara->pos.z = -60.0f;
            }
            break;
        case 3:
            CHARA_DAMP(chara, 0.92f);
            chara->vel.y += -0.004f;
            chara->vel.x += 0.05f * -chara->pos.x;
            chara->vel.y += 0.05f * (20.0f - chara->pos.y);
            chara->vel.z += 0.05f * (-60.0f - chara->pos.z);
            CHARA_INTEGRATE(chara);
            break;
        case 4:
            chara->unk4 = 5;
            chara->unk6 = 0;
            chara->vel.z = 0.0f;
        case 5:
            chara->vel.z += 7.5e-05f * chara->unk6;
            chara->pos.z += chara->vel.z;
            chara->unk6++;
            break;
        case 6:
            chara->unk4 = 7;
            chara->ape->flags &= ~0x20;
            chara->unkA = 3;
            chara->rotX = 0;
            chara->rotY = rand() / 32767.0f;
            chara->rotZ = 0;
            chara->pos.x = 0.0f;
            chara->pos.y = 0.0f;
            chara->pos.z = 0.0f;
            chara->vel.x = 0.0f;
            chara->vel.y = 0.0f;
            chara->vel.z = 0.0f;
            if (modeCtrl.playerCount == 1)
                chara->unk56 = 3;
            else
                chara->unk56 = (s16)modeCtrl.currPlayer;
            CHARA_RAND_ROT(chara);
        case 7:
            if (chara->unk6 > 0)
            {
                chara->unk6--;
                chara->vel.y += 0.009799992;
                CHARA_SPAWN_TRAIL(eff, chara, player);
            }
            else
                CHARA_DAMP(chara, 0.92f);
            CHARA_INTEGRATE(chara);
            chara->rotY -= 0x30;
            chara->unk5A -= 0x40;
            break;
        case 8:
            chara->vel.y += 0.009799992;
            chara->pos.y += chara->vel.y;
            chara->unk6--;
            if (chara->unk6 < 0)
            {
                chara->unk4 = 0;
                chara->ape->flags |= 0x20;
            }
            CHARA_SPAWN_TRAIL(eff, chara, player);
            break;
        case 9:
            chara->unk4 = 0xA;
            chara->ape->flags &= ~0x20;
            chara->unkA = 0x12;
            chara->rotX = 0;
            chara->rotY = (s16)((i << 16) / 3);
            chara->rotZ = 0;
            mtxa_from_rotate_y(chara->rotY + 0x8000);
            mtxa_tf_vec_xyz(0.0f, 0.0f, -5.0f, &chara->pos);
            chara->pos.y += 30.0f + 5.0f * (rand() / 32767.0f);
            chara->vel.x = 0.0f;
            chara->vel.y = -0.2f;
            chara->vel.z = 0.0f;
            if (i == active_monkey_id[player])
            {
                if (modeCtrl.playerCount == 1)
                    chara->unk56 = 3;
                else
                    chara->unk56 = (s16)i;
            }
            else
                chara->unk56 = (s16)(rand() & 3);
            CHARA_RAND_ROT(chara);
        case 0xA:
            raycast_stage_down(&chara->pos, &hit, NULL);
            if (chara->vel.y < 0.0f && 0.5f + (chara->pos.y - hit.pos.y) < -15.0f * chara->vel.y)
                chara->vel.y += 0.012f;
            CHARA_SPAWN_TRAIL(eff, chara, player);
        case 0xB:
            raycast_stage_down(&chara->pos, &hit, NULL);
            CHARA_DAMP(chara, 0.99f);
            chara->vel.y += -0.008f;
            CHARA_INTEGRATE(chara);
            if (hit.pos.y > chara->pos.y - 0.5f - 0.1f)
            {
                if (chara->vel.y < -0.1f)
                    call_SoundReqID_arg_0(0x69);
                chara->pos.y = 0.5f + hit.pos.y - 0.1f;
                if (chara->vel.y < 0.0f)
                    chara->vel.y *= -0.6f;
                if (chara->unk4 != 0xB)
                {
                    chara->unk4 = 0xB;
                    memset(&eff, 0, sizeof(eff));
                    eff.playerId = (s16)player;
                    eff.type = 0xA;
                    eff.pos = chara->pos;
                    eff.pos.y -= 0.4f;
                    eff.unk8C.x = 0.0f;
                    eff.unk8C.y = 1.0f;
                    eff.unk8C.z = 0.0f;
                    eff.colorFactor = 0.2f;
                    spawn_effect(&eff);
                }
            }
            break;
        case 0x10:
            chara->unk4 = 0;
            chara->unkA = 0xC;
            chara->pos = lbl_1_data_635B4[i];
            CHARA_SNAP_DOWN(chara, hit);
            chara->rotX = 0;
            chara->rotY = math_atan2(chara->pos.x, chara->pos.z);
            chara->rotZ = 0;
            chara->unk56 = -1;
            break;
        case 0x11:
            chara->unk4 = 0x12;
            chara->unkA = 0xD;
            if (active_monkey_id[player] == 3)
                chara->pos = lbl_1_data_635D8[i];
            else
                chara->pos = lbl_1_data_635D8[(i + 3 - active_monkey_id[player]) % 3];
            CHARA_SNAP_DOWN(chara, hit);
            chara->rotX = 0;
            chara->rotY = 0;
            chara->rotZ = 0;
            t = 1.0f / (float)modeCtrl.submodeTimer;
            chara->vel.x = -chara->pos.x * t;
            chara->vel.y = 0.0f;
            chara->vel.z = -4.5f * t * (1.0f + 0.1f * (rand() / 32767.0f));
            break;
        case 0x12:
            CHARA_INTEGRATE(chara);
            break;
        case 0x13:
            chara->unk4 = 0x14;
            chara->unk6 = (s16)modeCtrl.submodeTimer;
            chara->unkA = 0xD;
            chara->pos = lbl_1_data_635D8[i];
            chara->pos.y = 0.0f;
            chara->rotX = 0;
            chara->rotY = 0;
            chara->rotZ = 0;
            t = 1.0f / (float)chara->unk6;
            chara->vel.x = -chara->pos.x * t;
            chara->vel.y = 0.0f;
            chara->vel.z = (3.45f - chara->pos.z) * t;
            break;
        case 0x14:
            CHARA_INTEGRATE(chara);
            if (chara->pos.z < 3.45f)
            {
                chara->unk4 = 0;
                chara->unkA = 9;
                chara->pos.z = 3.45f;
            }
            break;
        case 0x15:
            chara->unk4 = 0;
            chara->unkA = 0xE;
            start = lbl_1_data_63578[i];
            chara->pos = start.pos;
            *(S16Vec *)&chara->rotX = start.rot;
            switch (i)
            {
            case 0:
                chara->unk3C = g_bg_gma->modelEntries[1].model;
                chara->unk40 = 0xA;
                chara->unk44.x = 0.05f;
                chara->unk44.y = 0.13f;
                chara->unk44.z = 0.120000005f;
                chara->unk50 = -0x58E3;
                chara->unk52 = -0x1555;
                chara->unk54 = 0x438E;
                break;
            case 1:
                chara->unk3C = g_bg_gma->modelEntries[0x17].model;
                chara->unk40 = 0xF;
                chara->unk44.x = 0.05f;
                chara->unk44.y = 0.0f;
                chara->unk44.z = -0.0940000042f;
                chara->unk50 = 0x20B7;
                chara->unk52 = -0x6BE1;
                chara->unk54 = -0x26E9;
                break;
            }
            break;
        }
    }
}

void fn_1_104BD4(void)
{
    int i;
    struct EndingChara *chara;

    if (lbl_1_bss_D3354.work != NULL)
    {
        chara = &lbl_1_bss_D3354.work->charas[3];
        for (i = 3; i >= 0; i--, chara--)
        {
            if (chara->ape != NULL)
            {
                fn_1_D29E0(lbl_1_bss_70E98[i]);
                chara->ape = NULL;
            }
        }
    }
}

struct EndingUnkE
{
    /*0x00*/ s8 bone;
    /*0x01*/ u8 r;
    /*0x02*/ u8 g;
    /*0x03*/ u8 b;
    /*0x04*/ u32 unk4;
    /*0x08*/ Vec offset;
    /*0x14*/ float scale;
}; /* size = 0x18 */

struct EndingUnkE lbl_1_data_63764[] = {
    { 0x00, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.320000023f },
    { 0x07, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0C, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x16, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x1B, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x15, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x1A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0F, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x18, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x13, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x08, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x0D, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x05, 0x23, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.100000001f }, 0.288000017f },
    { -1, 0x00, 0x00, 0x00, 0, { 0.0f, 0.0f, 0.0f }, 0.0f },
};
struct EndingUnkE lbl_1_data_638CC[] = {
    { 0x00, 0x18, 0x19, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.320000023f },
    { 0x07, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0C, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x16, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x1B, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x15, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x1A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0F, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x18, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x13, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x08, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x0D, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000057f },
    { 0x05, 0x20, 0x22, 0x23, 0, { 0.0f, 0.0f, 0.0399999991f }, 1.43999994f },
    { -1, 0x00, 0x00, 0x00, 0, { 0.0f, 0.0f, 0.0f }, 0.0f },
};
struct EndingUnkE lbl_1_data_63A34[] = {
    { 0x00, 0x20, 0x1B, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.320000023f },
    { 0x07, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0C, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x16, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x1B, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x15, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x1A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.160000011f },
    { 0x0F, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x0A, 0x17, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x18, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.0960000008f },
    { 0x13, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.0960000008f },
    { 0x08, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.0960000008f },
    { 0x0D, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.0f }, 0.0960000008f },
    { 0x05, 0x16, 0x18, 0x19, 0, { 0.0f, 0.0f, 0.200000003f }, 0.24000001f },
    { -1, 0x00, 0x00, 0x00, 0, { 0.0f, 0.0f, 0.0f }, 0.0f },
};
struct EndingUnkE lbl_1_data_63B9C[] = {
    { 0x00, 0x21, 0x25, 0x26, 0, { 0.0f, 0.0f, 0.0f }, 0.480000019f },
    { 0x07, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.24000001f },
    { 0x0C, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.24000001f },
    { 0x16, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.192000002f },
    { 0x1B, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.192000002f },
    { 0x15, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.24000001f },
    { 0x1A, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.24000001f },
    { 0x0F, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.256000012f },
    { 0x0A, 0x19, 0x1A, 0x1C, 0, { 0.0f, 0.0f, 0.0f }, 0.256000012f },
    { 0x18, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x13, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x08, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x0D, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.0f }, 0.128000006f },
    { 0x05, 0x25, 0x28, 0x29, 0, { 0.0f, 0.0f, 0.200000003f }, 0.496000022f },
    { -1, 0x00, 0x00, 0x00, 0, { 0.0f, 0.0f, 0.0f }, 0.0f },
};
struct EndingUnkE *lbl_1_data_63D04[] = {
    lbl_1_data_63764,
    lbl_1_data_638CC,
    lbl_1_data_63A34,
    lbl_1_data_63B9C,
};

void fn_1_1050D4(struct OrdTblNode *node);

struct EndingNode
{
    struct OrdTblNode node;
    struct EndingChara *chara;
};

void fn_1_104C6C(void)
{
    float t;
    Mtx m;
    Vec lightDir;
    Vec pos;
    Vec tl;
    int i;
    struct EndingChara *chara;
    struct Ape *ape;
    struct GMAModel *model;
    struct EndingUnkE *e;
    float f;

    mtxa_from_rotate_y(lbl_1_bss_2F48.infLightRotY);
    mtxa_rotate_x(lbl_1_bss_2F48.infLightRotX);
    mtxa_tf_vec_xyz(0.0f, 0.0f, -1.0f, &lightDir);
    chara = lbl_1_bss_D3354.work->charas;
    for (i = 4; i > 0; i--, chara++)
    {
        ape = chara->ape;
        if (ape == NULL)
            continue;
        model = chara->unk3C;
        if (model != NULL)
        {
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(m);
            mtxa_from_mtxb_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(m);
            mtxa_translate(&chara->unk44);
            mtxa_rotate_z(chara->unk54);
            mtxa_rotate_y(chara->unk52);
            mtxa_rotate_x(chara->unk50);
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_set_scale_factor(ape->scale);
            avdisp_draw_model_culled_sort_auto(model);
        }
        if (chara->unk56 >= 0)
        {
            struct OrdTblNode *entry;
            struct EndingNode *node;

            mtxa_from_mtxb();
            entry = ord_tbl_get_entry_for_pos(&chara->pos);
            node = ord_tbl_alloc_node(sizeof(*node));
            if (node != NULL)
            {
                node->node.drawFunc = fn_1_1050D4;
                node->chara = chara;
                fn_1_CD418(entry, &node->node);
            }
        }
        if (chara->unk36 & 1)
        {
            pos = chara->pos;
            mtxa_from_quat(&ape->rot);
            mtxa_to_mtx(m);
            mtxa_from_translate(&ape->pos);
            mtxa_scale_s(ape->scale);
            mtxa_translate(&ape->unk264);
            mtxa_mult_right(m);
            avdisp_set_z_mode(1, 3, 0);
            for (e = lbl_1_data_63D04[ape->charaId]; e->bone != -1; e++)
            {
                mtxa_push();
                mtxa_translate(&e->offset);
                mtxa_sq_from_identity();
                mathutil_mtxA_get_translate(&tl);
                t = tl.y - (pos.y - 0.4f);
                if (t > 0.6f)
                {
                    mtxa_pop();
                    continue;
                }
                f = -(t / lightDir.y);
                tl.x += lightDir.x * f;
                tl.y = pos.y - 0.4f;
                tl.z += lightDir.z * f;
                mathutil_mtxA_set_translate(&tl);
                mtxa_mult_left(mtxa->mtxB);
                mathutil_mtxA_get_translate(&tl);
                if (tl.z < -0.01f)
                {
                    t = 1.6666666f * t;
                    f = 0.003921569f * (1.0f - t);
                    set_post_mult_color(e->r * f, e->g * f, e->b * f, 1.0f);
                    t = e->scale + 0.2f * t;
                    f = (0.005f + tl.z) / tl.z;
                    tl.x *= f;
                    tl.y *= f;
                    tl.z *= f;
                    mathutil_mtxA_set_translate(&tl);
                    t *= f;
                    mtxa_scale_s(t);
                    mtxa_rotate_x(0x4000);
                    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                    avdisp_set_scale_factor(t);
                    avdisp_draw_model_culled_sort_always(init_common_gma->modelEntries[0x67].model);
                }
                mtxa_pop();
            }
            fade_color_base_default();
            avdisp_set_z_mode(1, 3, 1);
        }
    }
}

void fn_1_1050D4(struct OrdTblNode *node)
{
}

void fn_1_1051E4(struct Ape *ape, int b);

int fn_1_1050D8(int id, int a)
{
    struct EndingChara *chara = &lbl_1_bss_D3354.work->charas[id];
    struct Ape *ape;

    if (chara->ape != NULL)
    {
        fn_1_D29E0(lbl_1_bss_70E98[id]);
        chara->ape = NULL;
    }
    if (a >= 0)
    {
        chara->ape = fn_1_478C4(id, a, fn_1_1051E4);
        ape = chara->ape;
        if (ape == NULL)
            return 0;
        ape->flags |= 0x20;
        chara->unk8 = 0xC;
        chara->unkA = -1;
        mtxa_from_identity();
        mathutil_mtxA_get_translate(&ape->pos);
        mtxa_to_quat(&ape->rot);
    }
    return 1;
}

void fn_1_1051E4(struct Ape *ape, int b)
{
    struct EndingChara *chara;
    int i;
    int c = 0;
    int isThree = b == 3;

    if (isThree)
    {
        fn_1_D1F28(ape);
        if (b != 3)
            fn_1_D292C();
        return;
    }
    if (!(g_some_other_flags & 0xA))
    {
        chara = lbl_1_bss_D3354.work->charas;
        for (i = 4; i > 0; i--, chara++)
        {
            if (chara->ape == ape)
                break;
        }
        if (i != 0)
        {
            Vec v;
            int anim = chara->unk8;

            if (anim < 0x12)
                fn_1_D1F88(ape, 7, anim, 0.0f, c);
            else
            {
                anim -= 0x12;
                fn_1_D1F88(ape, 1, anim, 0.0f, anim + 1);
            }
            fn_1_D2510(ape);
            switch (chara->unk70)
            {
            case 2:
                g_ape_anim_head(ape, &chara->unk74);
                break;
            case 1:
                mtxa_from_mtx(cameras[modeCtrl.currPlayer].unk14C);
                mtxa_rigid_inv_tf_tl(&v);
                g_ape_anim_head(ape, &v);
                break;
            }
            mtxa_from_translate(&chara->pos);
            mtxa_rotate_z(chara->rotZ);
            mtxa_rotate_y(chara->rotY - 0x4000);
            mtxa_rotate_x(chara->rotX);
            mathutil_mtxA_get_translate(&ape->pos);
            mtxa_to_quat(&ape->rot);
        }
    }
}

void fn_1_1053B0(void)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0; i--, p++)
        p->active = 0;
}

#define BIT_RAND(p) (rand() / 32767.0f)

#define BIT_RAND_ROT(p) \
    do \
    { \
        (p)->rotX = (s16)(rand() & 0x7FFF); \
        (p)->rotY = (s16)(rand() & 0x7FFF); \
        (p)->rotZ = (s16)(rand() & 0x7FFF); \
    } while (0)

#define BIT_ADD_ROT(p) \
    do \
    { \
        (p)->rotX += (p)->rotVelX; \
        (p)->rotY += (p)->rotVelY; \
        (p)->rotZ += (p)->rotVelZ; \
    } while (0)

#define BIT_DAMP_ROT(p, k) \
    do \
    { \
        (p)->rotVelX = (k) * (p)->rotVelX; \
        (p)->rotVelY = (k) * (p)->rotVelY; \
        (p)->rotVelZ = (k) * (p)->rotVelZ; \
    } while (0)

#define BIT_SCALE_ALL(p) \
    do \
    { \
        (p)->scale.y = (p)->scale.x; \
        (p)->scale.z = (p)->scale.x; \
    } while (0)

#define BIT_PUSH(p, v, k) \
    do \
    { \
        (p)->pos.x += (k) * (v).x; \
        (p)->pos.y += (k) * (v).y; \
        (p)->pos.z += (k) * (v).z; \
        (p)->vel.x += 0.01f * (v).x; \
        (p)->vel.y += 0.01f * (v).y; \
        (p)->vel.z += 0.01f * (v).z; \
    } while (0)

void fn_1_1053E4(void)
{
    struct Effect effect;
    struct RaycastHit plane;
    Vec v;
    Vec dir;
    Vec tl;
    struct GMAModel *model;
    struct Camera *camera;
    int i;
    struct EndingBit *p;
    float t;
    float t2;
    struct EndingChara *chara;
    s16 anglePlus;

    fn_1_107188();
    camera = &cameras[modeCtrl.currPlayer];
    mtxa_from_mtx(camera->unk14C);
    mtxa_rigid_inv_tf_vec_xyz(0.0f, 0.0f, -1.0f, &dir);
    mtxa_rigid_inv_tf_tl(&tl);
    p = lbl_1_bss_D3354.work->bits;
    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active == 0)
            continue;
        model = p->model;
        switch (p->state)
        {
        case 2:
            t = p->pos.y;
            mtxa_from_rotate_y((g_font_char_var_3 << 10) + i);
            t2 = 0.25f * t * lbl_1_bss_D3354.work->timer;
            mtxa_tf_vec_xyz(t2, 0.0f, 0.0f, &v);
            mtxa_from_translate(&v);
            t2 = 1024.0f * t * lbl_1_bss_D3354.work->timer;
            mtxa_rotate_x(t2 * math_sin((g_font_char_var_3 << 11) + i));
            mtxa_rotate_z(t2 * math_sin((i + 0x4000) + (g_font_char_var_3 << 11)));
            mtxa_tf_point(&p->anchor->pos, &v);
            if (v.y < p->anchor->pos.y)
                v.y = p->anchor->pos.y;
            p->vel.x = 0.1f * (v.x - p->pos.x);
            p->vel.y = 0.1f * (v.y - p->pos.y);
            p->vel.z = 0.05f * (v.z - p->pos.z);
            p->rotVelX = 0;
            p->rotVelY = 0;
            p->rotVelZ = 0;
            if (p->timer != 0)
            {
                p->timer--;
                if (p->timer == 0)
                {
                    p->state = 3;
                    p->flags &= ~3;
                    p->flags |= 0x34;
                    if (i & 1)
                        p->flags |= 8;
                    vec_set_len(0.001f + 0.05f * BIT_RAND(p), &p->pos, &v);
                    p->vel.x += v.x;
                    p->vel.y += 0.05f + 4.0f * v.y;
                    p->vel.z += v.z;
                    p->rotVelX += 2048.0f * (BIT_RAND(p) - 0.5f);
                    p->rotVelY += 2048.0f * (BIT_RAND(p) - 0.5f);
                    p->rotVelZ += 2048.0f * (BIT_RAND(p) - 0.5f);
                }
            }
            break;
        case 4:
            p->state = 5;
            mtxa_from_rotate_y(rand() & 0x7FFF);
            mtxa_rotate_x(rand() & 0x7FFF);
            mtxa_tf_vec_xyz(0.0f, 0.0f, 30.0f * BIT_RAND(p), &v);
            v.y += 60.0f;
            t2 = mathutil_vec_dot_prod(&v, &dir);
            if (t2 < 0.0f)
            {
                v.x -= t2 * dir.x;
                v.y -= t2 * dir.y;
                v.z -= t2 * dir.z;
                t = mathutil_vec_len(&v);
                if (t > 80.0f)
                    vec_set_len(80.0f * BIT_RAND(p), &v, &v);
            }
            p->pos.x = tl.x + v.x;
            p->pos.y = tl.y + v.y;
            p->pos.z = tl.z + v.z;
            p->vel.x = 0.2f * (BIT_RAND(p) - 0.5f);
            p->vel.y = -0.2f * BIT_RAND(p);
            p->vel.z = 0.2f * (BIT_RAND(p) - 0.5f);
            BIT_RAND_ROT(p);
            p->rotVelX = 2048.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelY = 2048.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelZ = 2048.0f * (BIT_RAND(p) - 0.5f);
            p->scale.x = 0.3f * (1.0f + BIT_RAND(p));
            BIT_SCALE_ALL(p);
            p->unk48 = 0.0f;
            /* fallthrough */
        case 5:
            mtxa_from_mtx(camera->unk14C);
            mtxa_tf_point(&p->pos, &v);
            if (v.z > 5.0f)
            {
                p->state = 4;
                p->unk48 = 0.0f;
                break;
            }
            t = mathutil_vec_len(&v);
            if (t < 80.0f)
                p->unk48 += 0.1f * (1.1f - p->unk48);
            else
            {
                p->unk48 += 0.15f * (-0.1f - p->unk48);
                if (p->unk48 < 0.0f)
                {
                    p->state = 4;
                    p->unk48 = 0.0f;
                }
            }
            break;
        case 6:
            p->state = 7;
            p->flags |= 3;
            p->unk48 = 0.0f;
            p->scale.x = 2.0f * (1.0f + 0.5f * BIT_RAND(p));
            BIT_SCALE_ALL(p);
            BIT_RAND_ROT(p);
            p->rotVelX += 4096.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelY += 4096.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelZ += 4096.0f * (BIT_RAND(p) - 0.5f);
            t2 = p->timer;
            p->vel.x = 0.0f;
            p->vel.y = -0.2f * BIT_RAND(p);
            p->vel.z = 0.0f;
            p->unk3C.x = BIT_RAND(p) - 0.5f;
            p->unk3C.y = -0.5f * (t2 * (2.0f * p->vel.y + -0.008f * t2));
            p->unk3C.z = BIT_RAND(p) - 0.5f;
            /* fallthrough */
        case 7:
            BIT_DAMP_ROT(p, 0.99f);
            BIT_ADD_ROT(p);
            p->vel.y += -0.008f;
            p->unk3C.y += p->vel.y;
            if (p->unk3C.y <= 0.0f)
            {
                v.x = p->vel.x;
                v.y = p->vel.y;
                v.z = p->vel.z;
                camera_start_shake(modeCtrl.currPlayer, 0xF, &v);
                p->unk3C.y = 0.0f;
                if (p->vel.y < 0.0f)
                    p->vel.y *= -0.125f;
                p->rotVelX += 4096.0f * (BIT_RAND(p) - 0.5f);
                p->rotVelY += 4096.0f * (BIT_RAND(p) - 0.5f);
                p->rotVelZ += 4096.0f * (BIT_RAND(p) - 0.5f);
            }
            p->pos.x = camera->eye.x + p->unk3C.x;
            p->pos.y = p->unk3C.y + (camera->eye.y + 3.2f * p->scale.x);
            p->pos.z = camera->eye.z + p->unk3C.z;
            p->unk48 += 0.1f * (1.1f - p->unk48);
            break;
        case 8:
            p->state = 9;
            p->flags = 3;
            p->unk48 = 0.0f;
            p->scale.x = 0.4f;
            BIT_SCALE_ALL(p);
            BIT_RAND_ROT(p);
            p->rotVelX += 256.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelY += 16.0f * (BIT_RAND(p) - 0.5f);
            p->rotVelZ += 4096.0f * (BIT_RAND(p) + 0.5f);
            p->pos = lbl_1_bss_D3354.work->camera.lookAt;
            /* fallthrough */
        case 9:
            if (p->timer != 0)
                p->timer--;
            lbl_1_bss_D3354.work->camera.lookAt = p->pos;
            if (p->timer < 0x3C)
            {
                float invT = 1.0f / (p->timer + 1);

                p->vel.x = (0.15016f - p->pos.x) * invT;
                p->vel.y = (20.07f - p->pos.y) * invT;
                p->vel.z = (-60.0f - p->pos.z) * invT;
                CHARA_INTEGRATE(p);
                if (p->timer == 0)
                {
                    BIT_DAMP_ROT(p, 0.92f);
                    p->rotVelX += -0.1f * p->rotX;
                    p->rotVelY += -0.1f * p->rotY;
                    p->rotVelZ += -0.1f * p->rotZ;
                }
                BIT_ADD_ROT(p);
            }
            else
            {
                p->vel.x = 0.025f * (0.15016f - p->pos.x);
                p->vel.y = 0.025f * (20.07f - p->pos.y);
                p->vel.z = 0.0f;
                CHARA_INTEGRATE(p);
                BIT_ADD_ROT(p);
                memset(&effect, 0, sizeof(effect));
                effect.type = 4;
                effect.playerId = (s16)modeCtrl.currPlayer;
                effect.pos = p->pos;
                spawn_effect(&effect);
            }
            p->unk48 += 0.1f * (1.1f - p->unk48);
            break;
        case 10:
            chara = lbl_1_bss_D3354.work->charas;
            p->pos.x = 0.15016f + chara->pos.x;
            p->pos.y = 0.07f + chara->pos.y;
            p->pos.z = chara->pos.z;
            *(S16Vec *)&p->rotX = *(S16Vec *)&chara->rotX;
            break;
        }
        if (!(p->flags & 2))
        {
            CHARA_DAMP(p, 0.99f);
            BIT_DAMP_ROT(p, 0.984375f);
        }
        if (!(p->flags & 1))
        {
            p->vel.y += -0.008f;
            CHARA_INTEGRATE(p);
            BIT_ADD_ROT(p);
        }
        if (p->flags & 0x20)
        {
            t2 = model->boundSphereRadius * p->scale.x;
            t = 2.0f + t2;
            if (mathutil_vec_sq_len(&p->pos) < t * t)
            {
                vec_set_len(t, &p->pos, &v);
                v.x -= p->pos.x;
                v.y -= p->pos.y;
                v.z -= p->pos.z;
                BIT_PUSH(p, v, 0.025f);
            }
            v.x = p->pos.x;
            v.y = p->pos.y - 1.0f;
            v.z = p->pos.z - 3.0f;
            t = 1.5f + t2;
            if (mathutil_vec_sq_len(&v) < t * t)
            {
                vec_set_len(t, &v, &v);
                BIT_PUSH(p, v, 0.05f);
            }
            chara = &lbl_1_bss_D3354.work->charas[3];
            v.x = p->pos.x - chara->pos.x;
            v.y = p->pos.y - chara->pos.y;
            v.z = p->pos.z - chara->pos.z;
            t = 1.0f + t2;
            if (mathutil_vec_sq_len(&v) < t * t)
            {
                vec_set_len(t, &v, &v);
                BIT_PUSH(p, v, 0.05f);
            }
        }
        if (p->flags & 0x10)
        {
            float d;
            s16 angle;

            t2 = model->boundSphereRadius;
            plane.pos.x = 0.0f;
            plane.pos.y = -0.4f;
            plane.pos.z = 0.0f;
            plane.normal.x = 0.0f;
            plane.normal.y = 1.0f;
            plane.normal.z = 0.0f;
            mtxa_from_rotate_y(p->rotY);
            mtxa_rotate_x(p->rotX);
            mtxa_rotate_z(p->rotZ);
            mtxa_tf_vec_xyz(0.70710677f, 0.70710677f, 0.0f, &v);
            d = t2 * p->scale.x * (0.3f + 0.7f * __fabs(v.y));
            if (p->pos.y < plane.pos.y + d)
            {
                p->pos.y = plane.pos.y + d;
                t2 = mathutil_vec_dot_prod(&plane.normal, &p->vel);
                if (t2 < 0.0f)
                {
                    BIT_DAMP_ROT(p, 0.96875f);
                    mtxa_rigid_inv_tf_vec(&plane.normal, &v);
                    angle = math_atan2(-v.z, v.y);
                    anglePlus = angle + 0x4000;
                    p->rotVelX += (s16)((256.0 + 256.0 * __fabs(t2))
                        * math_sin(anglePlus + (s16)(((i << 11) & 0x1FFF) - 0xFFF)));
                    if (t2 < -0.1f)
                    {
                        t = 16384.0f * (0.1f + t2);
                        p->rotVelX += t * (BIT_RAND(p) - 0.5f);
                        p->rotVelY += t * (BIT_RAND(p) - 0.5f);
                        p->rotVelZ += t * (BIT_RAND(p) - 0.5f);
                    }
                    v = p->vel;
                    t2 *= -1.0f;
                    v.x += t2 * plane.normal.x;
                    v.y += t2 * plane.normal.y;
                    v.z += t2 * plane.normal.z;
                    v.x *= 0.9f;
                    v.y *= 0.9f;
                    v.z *= 0.9f;
                    t2 *= 0.6f;
                    v.x += t2 * plane.normal.x;
                    v.y += t2 * plane.normal.y;
                    v.z += t2 * plane.normal.z;
                    p->vel = v;
                }
            }
        }
    }
}

void fn_1_107188(void)
{
    Vec a;
    Vec b;
    Vec d;
    int i;
    int j;
    struct EndingBit *p;
    struct EndingBit *q;
    float mx;
    float my;
    float s;
    float ra;
    float rb;
    float t;
    float dx;
    float dy;
    float dz;

    p = lbl_1_bss_D3354.work->bits;
    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active == 0)
            continue;
        if (!(p->flags & 8))
            continue;
        ra = 0.5f * (p->model->boundSphereRadius * p->scale.x);
        a = p->pos;
        q = p + 1;
        for (j = i - 1; j > 0; j--, q++)
        {
            if (q->active == 0)
                continue;
            if (!(q->flags & 8))
                continue;
            rb = 0.5f * (q->model->boundSphereRadius * q->scale.x);
            b = q->pos;
            s = ra + rb;
            d.x = a.x - b.x;
            if (d.x < -s)
                continue;
            if (d.x > s)
                continue;
            d.z = a.z - b.z;
            if (d.z < -s)
                continue;
            if (d.z > s)
                continue;
            d.y = a.y - b.y;
            if (d.y < -s)
                continue;
            if (d.y > s)
                continue;
            if (mathutil_vec_distance(&a, &b) < s)
            {
                t = rb / s;
                mx = b.x + d.x * t;
                my = b.y + d.y * t;
                s = b.z + d.z * t;
                vec_normalize_len(&d);
                dx = mx + ra * d.x - a.x;
                dy = my + ra * d.y - a.y;
                dz = s + ra * d.z - a.z;
                a.x += 0.5f * dx;
                a.y += 0.5f * dy;
                a.z += 0.5f * dz;
                t = 0.25f / ra;
                p->vel.x += dx * t;
                p->vel.y += dy * t;
                p->vel.z += dz * t;
                dx = mx - rb * d.x - b.x;
                dy = my - rb * d.y - b.y;
                dz = s - rb * d.z - b.z;
                b.x += 0.5f * dx;
                b.y += 0.5f * dy;
                b.z += 0.5f * dz;
                t = 0.25f / rb;
                q->vel.x += dx * t;
                q->vel.y += dy * t;
                q->vel.z += dz * t;
                q->pos = b;
            }
        }
        p->pos = a;
    }
}

void fn_1_1075E0(void) {}

void fn_1_1075E4(void)
{
    Vec scale;
    Vec center;
    float radius;
    float alpha;
    float maxScale;
    int i;
    struct EndingBit *p;
    struct GMAModel *model;
    int isNl;

    p = lbl_1_bss_D3354.work->bits;
    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active == 0)
            continue;
        model = p->model;
        alpha = p->unk48;
        if (alpha <= 0.0f)
            continue;
        scale = p->scale;
        mtxa_from_mtxb_translate(&p->pos);
        mtxa_rotate_y(p->rotY);
        mtxa_rotate_x(p->rotX);
        mtxa_rotate_z(p->rotZ);
        mtxa_scale(&scale);
        maxScale = MAX(MAX(scale.x, scale.y), scale.z);
        isNl = p->flags & 0x40;
        if (isNl != 0)
        {
            radius = ((struct NlModel *)model)->boundSphereRadius;
            center = ((struct NlModel *)model)->boundSphereCenter;
        }
        else
        {
            radius = model->boundSphereRadius;
            center = model->boundSphereCenter;
        }
        if (!g_is_sphere_visible_scaled(&center, radius, maxScale))
            continue;
        if (p->flags & 4)
        {
            float depth = -(model->boundSphereRadius * maxScale) - mtxa->mtxA[2][3];

            if (depth < 0.0f)
                continue;
            if (depth < 1.0f)
                alpha *= depth;
        }
        if (isNl == 0)
        {
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_unculled_sort_always(model);
            }
            else
                avdisp_draw_model_unculled_sort_auto(model);
        }
        else
        {
            nl2ngc_set_scale(maxScale);
            if (alpha < 1.0f)
                nl2ngc_draw_model_alpha_sort_all((struct NlModel *)model, alpha);
            else
                nl2ngc_draw_model_sort_translucent((struct NlModel *)model);
        }
    }
}

void fn_1_107864(void)
{
    int i;
    int count;
    struct EndingBitAnchor *anchor;
    struct EndingBit *p;

    lbl_1_bss_D3354.flags |= 1;
    anchor = stagedef->unk80;
    count = stagedef->unk7C;
    p = lbl_1_bss_D3354.work->bits;
    for (i = 0x200; i > 0 && count > 0; i--, p++)
    {
        if (p->active == 0)
        {
            p->active = 1;
            p->state = 1;
            p->pos = anchor->pos;
            p->vel.x = 0.0f;
            p->vel.y = 0.0f;
            p->vel.z = 0.0f;
            p->rotX = 0;
            p->rotY = 0;
            p->rotZ = 0;
            mtxa_from_rotate_z(anchor->rotZ);
            mtxa_rotate_y(anchor->rotY);
            mtxa_rotate_x(anchor->rotX);
            mtxa_to_euler((S16Vec *)&p->rotX);
            p->scale = anchor->scale;
            p->unk48 = 1.0f;
            p->anchor = anchor;
            p->flags = 3;
            p->model = g_bg_gma->modelEntries[(anchor->flags & 1) ? 5 : 0].model;
            anchor++;
            count--;
        }
    }
}

void fn_1_107A18(void)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active != 0 && p->state == 1)
        {
            p->state = 2;
            p->timer = 0;
            p->flags &= ~1;
            p->flags |= 2;
        }
    }
}

void fn_1_107A88(void)
{
    Vec dir;
    float t;
    int i;
    struct EndingBit *p;

    mtxa_from_rotate_y((s16)((rand() & 0x3FF) - 0x200));
    mtxa_rotate_z((s16)((rand() & 0x7FFF) - 0x4000));
    mtxa_tf_vec_xyz(0.0f, -1.0f, 0.0f, &dir);
    p = lbl_1_bss_D3354.work->bits;
    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active != 0 && p->state == 2 && p->timer <= 0)
        {
            t = 1.0f + vec_dot_normalized_safe(&p->pos, &dir);
            p->timer = 1.0f + 60.0f * t * (0.9f + 0.2f * BIT_RAND(p));
        }
    }
}

void fn_1_107BD0(int count)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0 && count > 0; i--, p++)
    {
        if (p->active == 0)
        {
            p->active = 1;
            p->state = 4;
            p->flags = 0x14;
            p->model = g_bg_gma->modelEntries[(i & 1) ? 0 : 5].model;
            count--;
        }
    }
}

void fn_1_107C64(int count, int timer)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0 && count > 0; i--, p++)
    {
        if (p->active == 0)
        {
            p->active = 1;
            p->state = 6;
            p->timer = (s16)timer;
            p->model = g_bg_gma->modelEntries[(i & 1) ? 0 : 5].model;
            count--;
        }
    }
}

void fn_1_107CF8(int timer)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0; i--, p++)
    {
        if (p->active == 0)
        {
            p->active = 1;
            p->state = 8;
            p->timer = (s16)timer;
            p->unk48 = 0.0f;
            p->model = init_common_gma->modelEntries[0x41].model;
            break;
        }
    }
}

void fn_1_107D78(void)
{
    int i;
    struct EndingBit *p = lbl_1_bss_D3354.work->bits;

    for (i = 0x200; i > 0; i--, p++)
    {
        if ((p->state == 8 || p->state == 9) && p->active != 0)
        {
            p->state = 10;
            p->unk48 = 1.0f;
            break;
        }
    }
}

void fn_1_107DE8(void)
{
    Mtx m;
    GXTexObj tex;
    Vec eye;
    Vec up;
    Vec target;
    GXColor color;
    float x;
    float y;
    float negX;
    float negY;
    struct Ape *ape;
    struct EndingChara *chara;
    struct EndingWork *work = lbl_1_bss_D3354.work;

    if (work == NULL)
        return;
    if (work->unkAAA8 <= 0.0f)
        return;
    chara = work->charas;
    ape = chara->ape;
    if (ape == NULL)
        return;
    GXSetTexCopySrc_cached(0, 0, 0x280, 0x1C0);
    GXSetTexCopyDst_cached(0x280, 0x1C0, 4, 0);
    GXCopyTex(lbl_801988E0, 0);
    GXInitTexObj(&tex, lbl_801988E0, 0x280, 0x1C0, 4, 0, 0, 0);
    mtxa_from_quat(&ape->rot);
    mtxa_to_mtx(m);
    mtxa_from_translate(&ape->pos);
    mtxa_scale_s(ape->scale);
    mtxa_translate(&ape->unk264);
    mtxa_mult_right(m);
    mathutil_mtxA_get_translate(&target);
    mtxa_tf_point_xyz(0.0f, 0.0f, 0.1f, &target);
    mtxa_tf_point_xyz(work->unkAAAC, 0.0f, 0.2f, &eye);
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    C_MTXLookAt(mtxa->mtxB, &eye, &up, &target);
    chara->ape->flags &= ~0x20;
    g_draw_func_init();
    chara->ape->flags |= 0x20;
    GXSetNumChans_cached(0);
    GXSetNumTexGens_cached(1);
    GXSetNumTevStages_cached(1);
    GXLoadTexObj_cached(&tex, 0);
    color.r = 0xFF;
    color.g = 0xFF;
    color.b = 0xFF;
    color.a = (u8)(int)(255.0f * work->unkAAA8);
    GXSetTevKColor_cached(0, color);
    g_GXSetTevIndirect_zero_if_different(0);
    GXSetTevKAlphaSel_cached(0, 0x1C);
    GXSetTexCoordGen2_cached(0, 1, 4, 0x3C, 0, 0x7D);
    g_GXSetTevOrder_wrapper(0, 0, 0, 0xFF);
    GXSetTevSwapMode_cached(0, 0, 0);
    GXSetTevColorIn_cached(0, 0xF, 0xF, 0xF, 8);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 0, 1, 0);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetZMode_cached(1, 6, 0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    g_something_with_raster_state(0);
    GXSetCullMode_cached(0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    g_something_with_GX_vtx_desc(0x2200);
    x = -10.0f * (g_current_camera->sub28.view.tanHalfFov * g_current_camera->sub28.view.aspect);
    y = -10.0f * g_current_camera->sub28.view.tanHalfFov;
    negX = -x;
    negY = -y;
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, -10.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(negX, y, -10.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(negX, negY, -10.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x, negY, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    camera_apply_viewport(g_something_with_camera3);
}
