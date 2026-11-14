#ifndef BASEBALL_H
#define BASEBALL_H

#include "types.h"
#ifndef __MWERKS__
/* PC port: GCC scopes structs first named in a prototype to that prototype */
struct BBBallCtl;
struct BBCamMove;
struct BBCpu;
struct Ball;
struct Camera;
struct Sprite;
struct Item;
struct PhysicsBall;
struct Effect;
struct GMAModel;
struct Stage;
#endif
#include "ape_internal.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "dip.h"
#include "event.h"
#include "effect.h"
#include "frustum.h"
#include "gx.h"
#include "libc.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "os.h"
#include "pool.h"
#include "rand.h"
#include "sound.h"
#include "sprite.h"
#include "shadow.h"
#include "stage.h"
#include "stcoli.h"
#include "textdraw.h"
#include "world.h"

struct BBHelmet;
struct BBKey
{
    float t;
    float unk4;
    float unk8;
    float unkC;
};
struct BBTrack
{
    float unk0;
    s32 counts[3];
    struct BBKey *keys[3];
};
struct BBRules
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 filler6[2];
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u8 filler12[2];
    void (*unk14)();
    void (*unk18)();
    void (*unk1C)();
    s16 unk20;
    s16 unk22;
    u8 filler24[4];
};
struct BBWall
{
    Vec pos;
    S16Vec rot;
    u16 pad;
    Vec normal;
    float w;
    float h;
};

struct BBGate
{
    u8 unk0;
    u8 unk1;
    u8 filler2[6];
};

struct BBGateSet
{
    u8 unk0;
    u8 filler1[3];
    struct BBGate gates[7];
    u8 filler3C[4];
};

struct BBSub
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s16 unk4;
    s16 unk6;
    u32 unk8;
    u32 unkC;
};

struct BBBF0
{
    float unk0;
    float unk4;
    float unk8;
};

struct BBBFC
{
    u32 unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
};

struct BBC08
{
    u8 unk0;
    u8 unk1;
    s8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    float unk8;
    s16 unkC;
    float unk10;
    u32 unk14;
};

struct BBLayer
{
    u32 unk0;
    u16 unk4;
    void * DPTR unk8;   /* overlays the GMA TEV layers (GMAModel + 0x40) */
    u8 filler[0x14];
};

struct BBTex
{
    s16 *unk0;
    void *unk4[11];
    struct BBLayer *unk30[11];
};

struct BBC68Item
{
    u32 unk0;
    float unk4;
    s16 unk8;
    s16 unkA;
    s16 unkC;
    Vec unk10;
    Vec unk1C;
};

struct BBC68
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 unk4;
    struct BBC68Item unk8[6];
    u8 fillerF8[8];
};

struct BBSeries
{
    u8 unk0[4];
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 filler7[5];
};

struct BBPlayState
{
    void (*unk0)(void);
    void (*unk4)(void);
    void (*unk8)(void);
};

struct BBLift
{
    u8 unk0;
    u8 unk1;
    u32 unk4;
    u32 unk8;
    Vec unkC;
};

struct BBFieldPos
{
    Vec pos;
    float unkC;
    float unk10;
};

struct BBPlay;
typedef void (*BBPlayFn)(struct BBPlay *);

struct BBDebugMenu
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
};

struct BBSeq
{
    u8 unk0;
    u8 unk1;
    u32 unk4;
    u32 unk8;
};

struct BBTimer
{
    u32 unk0;
    u32 unk4;
    void (* DPTR unk8)(void);   /* overlays lbl_18_bss_970.unk220 (s32 words) */
};

struct BBBat
{
    u8 filler0[4];
    u8 state;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    float unk8;
    float unkC;
};

struct BBBatFuncs
{
    void (*unk0)(struct Ball *, struct BBBat *);
    void (*unk4)(struct Ball *, struct BBBat *);
    void (*unk8)(struct Ball *, struct BBBat *);
    void (*unkC)(struct Ball *, struct BBBat *);
};

struct BBBallFuncs
{
    void (*unk0)(struct Ball *);
    void (*unk4)(struct Ball *);
    void (*unk8)(struct Ball *);
};

struct BBGameReq
{
    u16 unk0;
    u16 unk2;
    u16 unk4;
};

struct BBGamePl
{
    u8 filler0[5];
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 filler9[3];
    u32 unkC;
    float unk10;
    float unk14;
};

struct BBScreenPair
{
    s32 unk0;
    s32 unk4;
};

struct BBScreenPos
{
    float x;
    float z;
};

struct BBScreen
{
    s32 unk0;
    u8 filler4[4];
    s32 unk8[14][2];
    u8 filler78[8];
    s32 unk80;
    u8 filler84[4];
    s32 unk88;
    s32 unk8C[2];
    u8 filler94[8];
    s32 unk9C[2];
    u8 fillerA4[0x28];
    s32 unkCC[3];
    s32 unkD8;
    s32 unkDC;
    s32 unkE0;
    float unkE4;
    float unkE8;
    struct BBScreenPos unkEC[7];
    struct BBScreenPos unk124[4];
    s32 unk144;
    s32 unk148;
    s32 unk14C;
    s32 unk150;
    s32 unk154;
    s32 unk158;
    s32 unk15C[0x3B];
    s32 unk248;
    s32 unk24C[0x3B];
    struct Sprite *unk338[0x80];
};

struct BBEnding
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    s8 unk3;
    s8 unk4;
    u8 unk5;
    u8 unk6;
    u32 unk8;
    u32 unkC;
};

struct CommendWork
{
    u8 filler0[4];
    struct Ape *apes[8];
    u8 filler24;
    s8 unk25;
};

struct BBAim
{
    u8 filler0[0x10];
    float unk10;
    float unk14;
};

struct BBGame
{
    /*0x000*/ u8 unk0;
    /*0x001*/ u8 unk1;
    /*0x002*/ u8 unk2;
    /*0x003*/ u8 unk3;
    /*0x004*/ u8 unk4;
    /*0x005*/ u8 unk5;
    /*0x006*/ u8 filler6[0x2];
    /*0x008*/ u8 unk8;
    /*0x009*/ u8 unk9;
    /*0x00A*/ u8 unkA;
    /*0x00B*/ u8 unkB;
    /*0x00C*/ u8 unkC;
    /*0x00D*/ u8 unkD;
    /*0x00E*/ u8 unkE;
    /*0x00F*/ u8 unkF;
    /*0x010*/ u8 unk10;
    /*0x011*/ u8 unk11;
    /*0x012*/ u16 unk12;
    /*0x014*/ u16 unk14;
    /*0x016*/ u8 filler16[0xB];
    /*0x021*/ u8 unk21;
    /*0x022*/ u8 unk22;
    /*0x023*/ u8 unk23;
    /*0x024*/ u8 unk24;
    /*0x025*/ u8 unk25;
    /*0x026*/ u8 unk26;
    /*0x027*/ u8 unk27;
    /*0x028*/ u8 unk28;
    /*0x029*/ u8 unk29;
    /*0x02A*/ u8 unk2A;
    /*0x02B*/ u8 unk2B;
    /*0x02C*/ u8 filler2C[0x19];
    /*0x045*/ u8 unk45;
    /*0x046*/ u8 unk46;
    /*0x047*/ u8 unk47;
    /*0x048*/ u8 filler48[0x30];
    /*0x078*/ u8 unk78[0x8];
    /*0x080*/ float unk80;
    /*0x084*/ float unk84;
    /*0x088*/ s32 unk88;
    /*0x08C*/ s32 unk8C;
    /*0x090*/ u8 unk90;
    /*0x091*/ u8 filler91[0x3];
    /*0x094*/ s32 unk94;
    /*0x098*/ u8 unk98;
    /*0x099*/ u8 unk99;
    /*0x09A*/ u8 unk9A;
    /*0x09B*/ u8 unk9B;
    /*0x09C*/ u8 unk9C;
    /*0x09D*/ u8 unk9D;
    /*0x09E*/ u8 filler9E[0x1];
    /*0x09F*/ u8 unk9F;
    /*0x0A0*/ u8 unkA0;
    /*0x0A1*/ u8 fillerA1[0x5];
    /*0x0A6*/ u8 unkA6;
    /*0x0A7*/ u8 fillerA7[0xF];
    /*0x0B6*/ u8 unkB6;
    /*0x0B7*/ u8 fillerB7[0x5];
    /*0x0BC*/ struct BBGamePl unkBC[0x6];
    /*0x14C*/ u8 filler14C[0x6C];
    /*0x1B8*/ float unk1B8;
    /*0x1BC*/ u8 filler1BC[0x30];
    /*0x1EC*/ u8 unk1EC;
    /*0x1ED*/ u8 unk1ED;
    /*0x1EE*/ u8 filler1EE[0x2];
    /*0x1F0*/ s32 unk1F0;
    /*0x1F4*/ s32 unk1F4;
    /*0x1F8*/ struct BBLift unk1F8;
    /*0x210*/ struct BBSub unk210;
    /*0x220*/ s32 unk220;
    /*0x224*/ s32 unk224;
    /*0x228*/ s32 unk228;
    /*0x22C*/ u8 filler22C[0x2];
    /*0x22E*/ struct BBGameReq unk22E[0x9];
    /*0x264*/ u8 filler264[0x4];
    /*0x268*/ u8 unk268;
    /*0x269*/ u8 filler269[0x3];
};  /* size = 0x26C */

struct BBCam
{
    /*0x000*/ u8 filler0[0x1C];
    /*0x01C*/ u8 unk1C;
    /*0x01D*/ u8 filler1D[0x3];
    /*0x020*/ s32 unk20;
    /*0x024*/ s32 unk24;
    /*0x028*/ s32 unk28;
    /*0x02C*/ s32 unk2C;
    /*0x030*/ s32 unk30;
    /*0x034*/ u8 filler34[0x4];
    /*0x038*/ s32 unk38;
    /*0x03C*/ u8 filler3C[0x4];
    /*0x040*/ s32 unk40;
    /*0x044*/ u8 unk44;
    /*0x045*/ u8 filler45[0x3];
    /*0x048*/ s32 unk48;
    /*0x04C*/ u8 unk4C;
    /*0x04D*/ u8 filler4D[0x6F];
    /*0x0BC*/ float unkBC;
    /*0x0C0*/ float unkC0;
    /*0x0C4*/ float unkC4;
    /*0x0C8*/ float unkC8;
    /*0x0CC*/ float unkCC;
    /*0x0D0*/ s16 unkD0;
    /*0x0D2*/ s16 unkD2;
    /*0x0D4*/ s16 unkD4;
    /*0x0D6*/ u16 unkD6;
    /*0x0D8*/ s32 unkD8;
    /*0x0DC*/ s32 unkDC;
    /*0x0E0*/ s32 unkE0;
    /*0x0E4*/ s32 unkE4;
    /*0x0E8*/ s32 unkE8;
    /*0x0EC*/ s32 unkEC;
    /*0x0F0*/ float unkF0;
    /*0x0F4*/ s16 unkF4;
    /*0x0F6*/ u8 fillerF6[0x2];
    /*0x0F8*/ float unkF8;
    /*0x0FC*/ u16 unkFC;
    /*0x0FE*/ s16 unkFE;
    /*0x100*/ s16 unk100;
    /*0x102*/ s16 unk102;
    /*0x104*/ float unk104;
    /*0x108*/ float unk108;
    /*0x10C*/ float unk10C;
    /*0x110*/ u8 filler110[0xC];
    /*0x11C*/ u32 unk11C;
    /*0x120*/ s32 unk120;
    /*0x124*/ u8 filler124[0x1A0];
    /*0x2C4*/ u32 unk2C4;
    /*0x2C8*/ s32 unk2C8;
    /*0x2CC*/ u8 filler2CC[0x3C];
    /*0x308*/ s32 unk308;
    /*0x30C*/ float unk30C;
    /*0x310*/ s32 unk310;
    /*0x314*/ u8 filler314[0x8];
    /*0x31C*/ s32 unk31C;
    /*0x320*/ u8 filler320[0x3C];
    /*0x35C*/ s32 unk35C;
    /*0x360*/ float unk360;
    /*0x364*/ s32 unk364;
    /*0x368*/ s16 unk368;
    /*0x36A*/ u8 filler36A[0x2];
    /*0x36C*/ float unk36C;
    /*0x370*/ u8 filler370[0x4];
    /*0x374*/ s16 unk374;
    /*0x376*/ u8 filler376[0x2];
    /*0x378*/ float unk378;
    /*0x37C*/ u8 filler37C[0x2];
    /*0x37E*/ u8 unk37E;
    /*0x37F*/ u8 filler37F[0x9];
    /*0x388*/ s32 unk388;
    /*0x38C*/ s32 unk38C;
    /*0x390*/ u8 filler390[0x50];
    /*0x3E0*/ s32 unk3E0;
    /*0x3E4*/ u8 filler3E4[0x50];
    /*0x434*/ s32 unk434;
    /*0x438*/ u8 filler438[0x50];
    /*0x488*/ s32 unk488;
    /*0x48C*/ u8 filler48C[0x50];
    /*0x4DC*/ s32 unk4DC;
    /*0x4E0*/ u8 filler4E0[0x50];
    /*0x530*/ s32 unk530;
    /*0x534*/ u8 filler534[0x50];
    /*0x584*/ s32 unk584;
    /*0x588*/ u8 filler588[0x50];
    /*0x5D8*/ s32 unk5D8;
    /*0x5DC*/ u8 filler5DC[0x5A];
    /*0x636*/ u8 unk636;
    /*0x637*/ u8 filler637[0x1];
    /*0x638*/ u8 unk638;
    /*0x639*/ u8 filler639[0x3];
    /*0x63C*/ s32 unk63C;
    /*0x640*/ s32 unk640;
    /*0x644*/ u8 filler644[0x30];
    /*0x674*/ s32 unk674;
    /*0x678*/ s32 unk678;
    /*0x67C*/ s32 unk67C;
    /*0x680*/ u8 filler680[0x6];
    /*0x686*/ s16 unk686;
    /*0x688*/ float unk688;
    /*0x68C*/ u8 filler68C[0x4];
};  /* size = 0x690 */

struct BBPlay
{
    /*0x000*/ s32 unk0;
    /*0x004*/ BBPlayFn unk4;
    /*0x008*/ Mtx unk8;
    /*0x038*/ Vec pos;
    /*0x044*/ Vec prevPos;
    /*0x050*/ s32 unk50;
    /*0x054*/ s32 colorId;
    /*0x058*/ float unk58;
    /*0x05C*/ u8 filler5C[0x8];
    /*0x064*/ struct Ape *unk64;
    /*0x068*/ Quat unk68;
    /*0x078*/ float unk78;
    /*0x07C*/ s16 unk7C;
    /*0x07E*/ s16 unk7E;
    /*0x080*/ s16 unk80;
    /*0x082*/ u8 unk82;
    /*0x083*/ u8 filler83[0x1];
};  /* size = 0x84 */


extern u8 lbl_18_data_1F8[0x170];
extern Vec lbl_18_data_368[4];
extern struct BBPlayState lbl_18_data_428[5];
extern void (* lbl_18_data_6A8[4])(void);
extern void (* lbl_18_data_6B8[3])(void);
extern void (* lbl_18_data_6C4[5])(void);
extern void (* lbl_18_data_6D8[5])(void);
extern u16 lbl_18_data_6EC[10];
extern struct BBBallFuncs lbl_18_data_A70[5];
extern struct BBBatFuncs lbl_18_data_AB0[8];
extern u16 lbl_18_data_B30[8][2];
extern struct BBFieldPos lbl_18_data_C48[8][4];
extern s32 lbl_18_data_78B50[2];
extern u8 lbl_18_data_78BC8[8][7];
extern struct BBTrack lbl_18_data_7F2B8[1];
extern struct BBTrack lbl_18_data_7F334[1];
extern struct BBTrack lbl_18_data_7F440[1];
extern struct BBCam lbl_18_bss_2E0;
extern struct BBGame lbl_18_bss_970;
extern struct BBSeries lbl_18_bss_BDC;
extern u8 lbl_18_bss_BE8[0x8];
extern struct BBBFC lbl_18_bss_BFC;
extern struct BBC08 lbl_18_bss_C08[4];
extern struct BBC68 lbl_18_bss_C68;
extern struct BBPlay lbl_18_bss_D68[6];
extern u8 lbl_18_bss_1080;
extern u8 lbl_18_bss_1088;
extern u8 lbl_18_bss_108C;
extern struct BBTex lbl_18_bss_1090[3];
extern struct BBGateSet lbl_18_bss_11A8;
extern struct BBScreen * lbl_18_bss_11E8;
extern struct BBRules lbl_18_bss_11F0;
extern u32 lbl_18_bss_1218;
extern s32 lbl_18_bss_121C;
extern s32 lbl_18_bss_1220[8];
extern struct BBScreen * lbl_18_bss_19270;
extern float lbl_18_bss_19274;
extern struct BBScreen lbl_18_bss_19278;
extern struct BBEnding lbl_18_bss_1A2C0;

void PSVECScale(Vec *src, Vec *dst, float scale);
void PSVECAdd(Vec *a, Vec *b, Vec *dst);
void PSVECSubtract(Vec *a, Vec *b, Vec *dst);
extern int currentHeap;
int DVDChangeDir(char *dir);
int DVDClose(struct DiscFileInfo *file);
int DVDOpen(char *path, struct DiscFileInfo *file);
void GXSetCopyClear_cached(GXColor color, u32 z);
u32 OSCheckHeap(int);
u32 *OSGetStackPointer(void);
void call_free_bmp_by_id(int id);
void collide_ball_with_circle(struct PhysicsBall *ball, struct ColiCircle *circle);
void collide_ball_with_plane(struct PhysicsBall *ball, struct ColiPlane *plane);
void create_pausemenu_sprite(void);
void create_stage_player(u8 playerIdx, int mode, u8 apeChara, u8 colorId, u8 lod, void (*drawFunc)(struct Ape *), int variant);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void dest_all_events(void);
void destroy_sprite_with_unique_id(int tag);
void draw_ape(struct Ape *ape);
void draw_shadow_reqs(void);
void event_dest(int id);
void event_init(int id);
void event_vibration_dest(void);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_10A2C0(struct GMAModel *model);
void fn_1_10A300(struct GMAModel *model);
void fn_1_16373C(Vec *pos);
void fn_1_1637B4(int cam, Mtx mtx);
void fn_1_1639BC(void);
void fn_1_1639C0(int cam);
void fn_1_163ABC(int cam);
void fn_1_163DAC(Vec *pos);
void fn_1_1641E4(Vec *pos, Vec *vel);
void fn_1_164B90(int type, Vec *pos, Vec *normal, float scale);
void fn_1_167028(Vec *pos, Vec *vel);
void fn_1_167E74(Vec *pos);
void fn_1_168968(Vec *pos, Vec *vel, Vec *normal);
void fn_1_169660(void);
void fn_1_169940(void);
void fn_1_45CA8(struct Ape *ape);
void fn_1_47444(u8 playerIdx);
void fn_1_474D0(void);
void fn_1_4AF48(struct Ball *ball, struct PhysicsBall *physBall);
void fn_1_4EAFC(Mtx m, int color, int c);
void fn_1_5BC10(void);
int fn_1_A74E4(Vec *a, Vec *b, float ra, Vec *c, Vec *d, float rb);
void fn_1_E4708(s8 *charas, s8 *ranks);
u32 fn_1_E5CD0(void);
int fn_1_E5DC8(void);
void fn_1_E6760(Mtx mtx);
void fn_1_E67BC(int a);
struct CommendWork *fn_1_E7668(void);
int fn_1_F004C(void);
void fn_80066BE0(u32 lightMask);
void g_avdisp_set_ambient(float r, float g, float b);
void g_call_synthFXSetCtrl_with_irq_disabled(u32 vid, u8 ctrl, u8 value);
int g_check_some_condition(int a);
void g_draw_ball_and_ape(void);
void g_fade_track_volume(int a, int b);
void g_idle_function(int);
void g_init_shadows(void);
struct Ape *g_load_ape_gameplay_variant0(int a, int lod);
void g_read_inputs_and_maybe_reset_console(void);
void g_reset_ball(struct Ball *ball);
void g_return_to_sel_ngc(int a);
void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void *menu, void *camera);
void g_something_with_GXPeekZ(void);
void g_something_with_drawing_apes(void);
void g_something_with_graphics_processor2(void);
void g_something_with_music_fading(void);
void g_something_with_pause_menu_and_game_over_sprite(s8 *status, struct Sprite *sprite);
void g_something_with_replays3(void);
void g_very_similar_to_puts(const char *);
struct Sprite *get_sprite_with_unique_id(int tag);
void gp_wait(void);
void gx_finish_frame_and_reset_z_and_alpha_compare_modes(void);
void gx_start_new_frame(void);
void init_itemgroups(void);
void init_shadow_reqs(void);
void load_bmp_by_id(int id);
void load_stage_models(void);
void mtxa_to_quat(Quat *q);
void mtxa_translate_neg_xyz(float x, float y, float z);
void ord_tbl_draw_nodes(void);
void position_ball(struct Ball *ball, struct PhysicsBall *physBall);
void ray_to_euler_xy(Vec *, Vec *, s16 *, s16 *);
void reset_ui_widescreen_scale_mtx(void);
void revert_console_reset_callback(void);
void set_console_reset_callback(int (*)(void));
void set_ui_widescreen_scale_mtx(int width);
u32 sndFXCheck(u32 vid);
int sndFXKeyOff(u32 vid);
int sndPopGroup(void);
int sndPushGroup(void *proj, u16 groupId, void *samp, void *sdir, void *pool);
void textdraw_clear_drop_shadow(void);
float textdraw_get_pixel_width_of_string(char *str);
void textdraw_print(char *str);
void textdraw_print_and_fit_to_width(float width, char *text);
void textdraw_printf_and_fit_to_width(float width, char *format, ...);
void textdraw_reset(void);
void textdraw_set_alignment(s8 alignment);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_drop_shadow(void);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_font_style(int style);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void tick_events(void);
void video_finish_frame(void);
u32 AIGetStreamSampleCount(void);
void DTKFlushTracks(void (*cb)(void));
void DTKInit(void);
u32 DTKNextTrack(void);
u32 DTKPrevTrack(void);
u32 DTKQueueTrack(char *name, void *info, u32 mode, void *cb);
u32 DTKSetState(u32 state);
void DTKSetVolume(u8 l, u8 r);
s32 DVDReadPrio(void *file, void *buf, s32 len, s32 ofs, s32 prio);
void fn_80061AF0(void);
u32 g_DTK_get_something1(void);
void g_DTK_set_something(u32 a);
u32 g_GTK_get_something2(void);
void g_math_unk14(s16 *a, s16 *b, s16 *c);
u32 sndFXStartEx(s32 fid, u8 vol, u8 pan, u8 studio);
int (*get_console_reset_callback(void))(void);
struct PauseMenuState
{
    s32 timer;
    u32 flags;
    s32 selection;
    s32 itemCount;
    s32 menuType;
    s8 padId;
    s8 playerId;
    s16 unk16;
    s8 screenshotState;
};
struct ScreenFadeInfo
{
    s32 type;
    u32 color;
    s32 timer;
    s32 timerMax;
};
void fn_1_10A3A0(Vec *dir);
void fn_1_10A410(float r, float g, float b);
void fn_1_113190(struct Ape *ape, int a, float speed, int b, int c);
float fn_1_1135E8(struct Ape *ape);
float fn_1_45A8C(struct Ape *ape);
void g_ape_anim_body(struct Ape *ape);
void *g_get_ape_anim_smth(struct Ape *ape, int bone);
extern s32 main_heap, stage_heap, bg_heap, chara_heap, replay_heap;
extern s32 locale;
extern struct PauseMenuState g_related_to_pause_menu_input;
extern struct ScreenFadeInfo g_screenfade_flags;
extern u32 g_player_id_for_sound;
extern u32 g_smth_with_sound;

void fn_18_130(void);
s32 fn_18_1FC(void);
void fn_18_25C(void);
void fn_18_270(void);
void fn_18_274(void);
s32 fn_18_28C(void);
void fn_18_2B4(void);
s32 fn_18_334(void);
void fn_18_344(s32 arg0);
void fn_18_354(void);
void fn_18_390(void);
void fn_18_394(void);
void fn_18_398(void);
void fn_18_39C(void);
void fn_18_3BC(void);
void fn_18_3C0(void);
void fn_18_4A8(void);
void fn_18_4D0(void);
s32 g_baseball_game_loop(void);
void fn_18_5E0(void);
void fn_18_640(void);
void fn_18_680(void);
void fn_18_6BC(void);
void fn_18_6DC(void);
void fn_18_744(void);
void fn_18_748(void);
void fn_18_7AC(void);
void fn_18_878(void);
void fn_18_944(void);
void fn_18_988(void);
void fn_18_9F4(struct Ape *ape);
void fn_18_C4C(struct Ape *ape);
void fn_18_E9C(void);
void fn_18_ECC(u8 arg0, u8 arg1);
void fn_18_FB8(u8 arg0, u8 arg1);
void fn_18_1010(u8 arg0, s8 *arg1, s8 *arg2);;
void fn_18_1040(u8 arg0, u8 arg1);
void fn_18_10A0(u8 arg0, u16 *arg1);
void fn_18_10C4(void);
void fn_18_1240(s8 *arg0, s8 *arg1, u16 *arg2);;
void fn_18_1274(void);
void fn_18_13F0(s8 *arg0, s8 *arg1, u16 *arg2);;
void fn_18_1424(void);
void fn_18_146C(u8 arg0, u16 arg1, u16 arg2);;
void fn_18_14A0(u8 arg0, u16 arg1);;
void fn_18_14D8(u8 arg0, u16 arg1);;
void fn_18_1510(u8 arg0, u16 arg1);;
void fn_18_1548(u8 arg0, u16 *arg1, u16 *arg2);
s32 fn_18_1584(struct Ape *arg0);;
float fn_18_15BC(float farg0, float *arg0, float *arg1);;
float fn_18_1614(float *arg0);
void fn_18_16D4(u8 arg0);
u8 fn_18_16E8(void);
void fn_18_16FC(u8 arg0);
u8 fn_18_1798(void);
void fn_18_17AC(u8 arg0, u8 arg1);
u8 fn_18_17C4(u8 arg0);
u8 fn_18_1818(u8 arg0);
u8 fn_18_18CC(u8 arg0);;
void fn_18_196C(u8 arg0);
u8 fn_18_1980(void);
void fn_18_1994(u8 arg0);
u8 fn_18_19B8(void);
u8 fn_18_19CC(void);
u8 fn_18_19E0(void);
void fn_18_19F4(s32 arg0);
void fn_18_1A04(s32 arg0);
u32 fn_18_1A14(void);;
void fn_18_1A24(void *arg0, void *arg1, void *arg2, float farg0);
s32 fn_18_1A7C(void);
s32 fn_18_1A9C(void);
s32 fn_18_1B50(void);
u8 fn_18_1B80(void);
void fn_18_1B90(void);
s16 fn_18_1F40(s32 arg0, s32 arg1, s16 arg2, s16 arg3);
void fn_18_1FB0(void);
void fn_18_24B4(u8 arg0);
void fn_18_2504(void);
void fn_18_264C(u8 arg0);
void fn_18_2724(u8 arg0);
void fn_18_27A0(u32 arg0, s32 arg1);
void fn_18_2828(u32 arg0, s32 arg1);
void fn_18_28B0(void);
void fn_18_2930(void);
void fn_18_297C(void);
void fn_18_2980(void);
void fn_18_3004(void);
void fn_18_3134(void);
void fn_18_3154(void);
void fn_18_3178(void);
void fn_18_329C(void);
void fn_18_3304(void);
void fn_18_3344(void);
void fn_18_33C8(void);
void fn_18_3430(void);
void fn_18_34C0(void);
void fn_18_34E8(void);
void fn_18_3510(void);
void fn_18_3578(void);
void fn_18_364C(void);
void fn_18_3728(u8 arg0);
void fn_18_37E0(void);
void fn_18_38B0(void);
void fn_18_3988(void);
void fn_18_3A90(void);
void fn_18_3B28(void);
void fn_18_3BB0(void);
s32 fn_18_3BD4(void);
void fn_18_3BF4(void);
void fn_18_3E44(void);
void fn_18_3E78(void);
u32 fn_18_3EF4(void);;
u8 fn_18_3F08(void);
void fn_18_3F1C(struct BBSub *arg0);
void fn_18_3F64(struct BBSub *arg0);
void fn_18_3F68(struct BBSub *arg0);
void fn_18_4038(struct BBSub *arg0);
void fn_18_4058(struct BBSub *arg0);
void fn_18_405C(struct BBSub *arg0);
void fn_18_423C(struct BBSub *arg0);
void fn_18_4254(struct BBSub *arg0);
void fn_18_4258(struct BBSub *arg0);
void fn_18_4318(struct BBSub *arg0);
void fn_18_433C(struct BBSub *arg0);
void fn_18_4340(struct BBSub *arg0);
void fn_18_44BC(struct BBSub *arg0);
void fn_18_44E0(struct BBSub *arg0);
void fn_18_4500(struct BBSub *arg0);
void fn_18_45D4(struct BBSub *arg0);
void fn_18_45F8(struct BBSub *arg0);
void fn_18_45FC(struct BBSub *arg0);
void fn_18_4848(struct BBSub *arg0);
void fn_18_48BC(struct BBSub *arg0);
void fn_18_48DC(struct BBSub *arg0);
void fn_18_4A7C(struct BBSub *arg0);
void fn_18_4B40(struct BBSub *arg0);
void fn_18_4B60(struct BBSub *arg0);
void fn_18_4D78(struct BBSub *arg0);
void fn_18_4E00(struct BBSub *arg0);
void fn_18_4E24(struct BBSub *arg0);
void fn_18_4F48(struct BBSub *arg0);
void fn_18_5004(struct BBSub *arg0);
void fn_18_502C(struct BBSub *arg0);
void fn_18_5294(struct BBSub *arg0);
void fn_18_52DC(struct BBSub *arg0);
void fn_18_52E0(struct BBSub *arg0);
void fn_18_560C(struct BBSub *arg0);
void fn_18_5610(struct BBSub *arg0);
void fn_18_571C(struct BBSub *arg0);
void fn_18_5720(struct BBSub *arg0);
void fn_18_5774(struct BBSub *arg0);
void fn_18_5794(struct BBSub *arg0);
void fn_18_5950(struct BBSub *arg0);
void fn_18_5A08(struct BBSub *arg0);
void fn_18_5A28(struct BBSub *arg0);
void fn_18_5BAC(struct BBSub *arg0);
void fn_18_5CF8(struct BBSub *arg0);
void fn_18_5D5C(struct BBPlay *arg0);
void fn_18_5DCC(struct BBSub *arg0);
void fn_18_6288(struct BBPlay *arg0);
void fn_18_6504(struct BBPlay *arg0);
void fn_18_6828(struct BBPlay *arg0);
s8 fn_18_6B94(u8);
void fn_18_6C98(struct BBSub *arg0);
void fn_18_6CC0(struct BBSub *arg0);
void fn_18_6CC4(struct BBSub *arg0);
void fn_18_6DBC(struct BBSub *arg0);
void fn_18_6DC0(struct BBSub *arg0);
void fn_18_6DC4(struct BBSub *arg0);
void fn_18_6EB8(void);
void fn_18_7070(struct BBSub *arg0);
void fn_18_70D0(struct BBSub *arg0);
void fn_18_70D4(struct BBSub *arg0);
s8 fn_18_7270(void);
void fn_18_72B8(struct BBSub *arg0);
void fn_18_7318(struct BBSub *arg0);
void fn_18_731C(struct BBSub *arg0);
void fn_18_7408(struct BBPlay *arg0);
void fn_18_7574(struct BBPlay *arg0);
void fn_18_7818(struct BBPlay *arg0);
void fn_18_78E4(struct BBPlay *arg0);
void fn_18_7AA8(struct BBSub *arg0);
void fn_18_7CD4(struct BBSub *arg0);
u8 fn_18_7CD8(void);
void fn_18_7CE8(struct BBSub *arg0);
u8 fn_18_81B0(void);
void fn_18_8324(void);
void fn_18_8374(struct Ball *arg0);;
void g_load_baseball(u8 arg0, s32 arg1, s8 arg2, u8 arg3, u8 arg4, u8 arg5, void (*arg6)(struct Ape *), s32 arg7);
void fn_18_84D4(struct Ball *arg0);;
void fn_18_8558(struct Ball *arg0);;
void fn_18_8784(struct Ball *arg0);;
void fn_18_87F8(struct BBPlay *arg0);;
void fn_18_89C0(struct BBPlay *arg0);;
void fn_18_8A34(u8 arg0, s32 arg1, s32 arg2, s32 arg3, BBPlayFn arg4);;
void fn_18_8B70(void);
void fn_18_8C04(void);
void fn_18_8CD4(u8 arg0);
void fn_18_8D3C(void);
void fn_18_8DC0(struct Ball *arg0);;
void fn_18_8E30(void);
void fn_18_8E34(struct Ball *arg0);;
void fn_18_8EB0(struct Ball *arg0, void *arg1);;
void fn_18_8EE0(struct Ball *arg0);;
u8 fn_18_8FAC(void);
void fn_18_8FC0(struct Ball *arg0);;
void fn_18_91A4(void);
void fn_18_91A8(void);
void fn_18_91AC(struct Ball *arg0);
void fn_18_9294(struct Ball *arg0, struct BBBat *arg1);
void fn_18_92E4(void);
void fn_18_92E8(struct Ball *arg0, struct BBBat *arg1);
void fn_18_937C(struct Ball *arg0);
void fn_18_9474(struct Ball *arg0);
void fn_18_94B4(void);
void fn_18_94B8(void);
void fn_18_94BC(struct Ball *arg0);
void fn_18_95B8(struct Ball *arg0);
void fn_18_95F8(void);
void fn_18_95FC(struct Ball *arg0, struct BBBat *arg1);
void fn_18_9730(struct Ball *arg0);
void fn_18_9830(struct Ball *arg0);
void fn_18_985C(void);
void fn_18_9860(struct Ball *arg0);
void fn_18_995C(void);
void fn_18_9960(struct Ball *arg0);
void fn_18_99A8(void);
void fn_18_99AC(void);
void fn_18_99B0(struct Ball *arg0);
void fn_18_9AD4(struct Ball *arg0);
void fn_18_9B00(void);
void fn_18_9B04(void);
void fn_18_9B08(struct Ball *arg0);
void fn_18_9C0C(struct Ball *arg0);
void fn_18_9C40(void);
void fn_18_9C44(void);
void fn_18_9C48(struct Ball *arg0);
void fn_18_9E20(u8 arg0, u16 arg1);;
void fn_18_9E88(struct Ball *arg0);;
void fn_18_9FC8(void);
u8 fn_18_A000(void);
s32 fn_18_A014(struct Ball *arg0, struct BBBat *arg1, s8 arg2, s8 arg3);;
s32 fn_18_A1E4(void);
s32 fn_18_A210(s16 arg0);
void fn_18_A248(s32 arg0);
void fn_18_A2F4(struct Ball *ball);
void fn_18_A368(struct Ball *arg0);
void fn_18_A3D8(void);
void fn_18_A3DC(struct Ball *arg0);
void fn_18_A458(struct Ball *arg0, struct PhysicsBall *arg1);
void fn_18_A784(struct Ball *arg0);
void fn_18_A860(s32 arg0, u8 arg1);
u8 fn_18_A94C(void);
void fn_18_A960(struct Ball *arg0);
void fn_18_AAC0(void);
void fn_18_AAC4(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_AC08(void);
void fn_18_AC0C(struct Ball *arg0);
void fn_18_AC4C(void);
void fn_18_AC50(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_ACA8(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_ADD8(struct Ball *arg0);
void fn_18_AE04(void);
void fn_18_AE08(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_AE60(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_B190(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_B1E0(void);
void fn_18_B1E4(void);
void fn_18_B1E8(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_B5C0(struct Ball *arg0, struct BBBallCtl *arg1);
void fn_18_B61C(void);
void fn_18_B620(void);
void fn_18_B624(struct Ball *arg0);
void fn_18_B70C(void);
void fn_18_B710(void);
void fn_18_B714(void);
void fn_18_B718(struct Ball *arg0);
void fn_18_B8DC(struct Ball *arg0);
void fn_18_B91C(void);
void fn_18_B920(void);
void fn_18_B924(struct Ball *arg0);
void fn_18_BA1C(void);
void fn_18_BA40(void);
void fn_18_BA44(void);
void fn_18_BA48(struct Ball *arg0);
void fn_18_BB5C(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3);
void fn_18_BCA4(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3);
void fn_18_BDF0(struct Ball *arg0, struct BBBallCtl *arg1, u8 arg2, u8 arg3);
void fn_18_BF20(struct Ball *arg0);
void fn_18_BFE0(void);
void fn_18_C038(struct PhysicsBall *arg0);
s32 fn_18_C09C(void);
s32 fn_18_C0C8(void);
u8 fn_18_C0F4(void);
u8 fn_18_C124(void);
float fn_18_C154(void);
s32 fn_18_C190(s16 arg0);
void fn_18_C1C8(s32 arg0);
void fn_18_C244(s32 arg0);
u8 fn_18_C278(void);
void fn_18_C28C(struct Ball *arg0, struct BBBallCtl *arg1);
u8 fn_18_C394(void);
u8 fn_18_C3A8(void);
void fn_18_C3BC(void);
void fn_18_C3D4(struct Ball *arg0);
void fn_18_C47C(void);
void fn_18_C480(struct Ball *arg0);
void fn_18_C518(struct Ball *arg0);
u8 fn_18_C5F4(s32 arg0);
void fn_18_C610(struct Ball *arg0, struct PhysicsBall *arg1);
void fn_18_C640(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_C900(void);
void fn_18_C904(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_CA04(struct Ball *arg0);
void fn_18_CBD8(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_CBE4(void);
void fn_18_CBE8(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_CC6C(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_D178(struct Ball *arg0);
void fn_18_D1A4(void);
void fn_18_D1A8(void);
void fn_18_D1AC(struct Ball *arg0, struct BBGamePl *arg1);
s32 fn_18_D390(void);
void fn_18_D3D0(struct Ball *arg0);
void fn_18_D3FC(void);
void fn_18_D400(void);
void fn_18_D404(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_D758(struct Ball *arg0, struct BBGamePl *arg1);
void fn_18_D7F0(struct Ball *arg0);
void fn_18_D9A4(struct PhysicsBall *arg0, Vec *arg1, float farg0, float farg1);
void fn_18_DA58(struct Ball *arg0);
void fn_18_DA84(void);
void fn_18_DA88(void);
void fn_18_DA8C(struct Ball *arg0);
s32 fn_18_DB8C(struct Ball *arg0, struct BBGamePl *arg1, s32 arg2, s32 arg3);
void fn_18_DD38(struct Ball *ball);
void fn_18_DE1C(struct Ball *ball, u8 arg1, struct BBAim *aim);
void fn_18_DF60(void);
void fn_18_E06C(u8 arg0);
u8 fn_18_E0AC(void);
void fn_18_E0C0(s32 arg0);
void fn_18_E17C(struct Ball *ball);
void fn_18_E3E4(void);
void fn_18_E3E8(struct Ball *ball);
void fn_18_E540(u8 arg0);
void fn_18_E550(struct Ball *ball);
void fn_18_E794(void);
void fn_18_E798(struct Ball *ball);
s32 fn_18_E7EC(void);
s32 fn_18_E838(void);
void fn_18_E8C8(struct Ball *ball);
void fn_18_E9A0(s32 arg0, s8 arg1);
void fn_18_EA54(struct Camera *arg0, struct Ball *arg1);
void fn_18_EAE4(s32 arg0, s8 arg1);
u8 fn_18_EBDC(void);
void fn_18_EBF4(struct Camera *arg0);
void fn_18_EC7C(void);
void fn_18_EC80(void);
void fn_18_EC84(void);
void fn_18_EC88(void);
void fn_18_EC8C(struct Camera *arg0, struct Ball *arg1);
void fn_18_ED94(struct Camera *arg0);
void fn_18_EE1C(void);
void fn_18_EE20(void);
void fn_18_EE24(struct Camera *arg0, struct Ball *arg1);
void fn_18_EE8C(void);
void fn_18_EE90(struct Camera *arg0, struct Ball *arg1);
void fn_18_F024(void);
void fn_18_F028(void);
void fn_18_F02C(struct Camera *arg0, struct Ball *arg1);
void fn_18_F294(struct Camera *arg0);
void fn_18_F31C(void);
void fn_18_F320(struct Camera *arg0);
void fn_18_F420(void);
void fn_18_F424(void);
void fn_18_F428(struct Camera *arg0, struct Ball *arg1);
void fn_18_F58C(void);
void fn_18_F590(void);
void fn_18_F594(struct Camera *arg0);
void fn_18_F85C(struct Camera *arg0);
void fn_18_F990(void);
void fn_18_F994(struct Camera *arg0);
void fn_18_FAC8(struct Camera *arg0);
void fn_18_FB1C(void);
void fn_18_FB20(struct Camera *arg0);
void fn_18_FC7C(void);
void fn_18_FC80(void);
void fn_18_FC84(void);
void fn_18_FC88(struct Camera *arg0);
void fn_18_FD10(void);
void fn_18_FD14(void);
void fn_18_FD18(struct Camera *arg0);
void fn_18_FD94(void);
void fn_18_FD98(struct Camera *arg0);
void fn_18_FF58(struct Camera *arg0);
void fn_18_FF8C(void);
void fn_18_FF90(struct Camera *arg0);
void fn_18_1016C(struct Camera *arg0);
void fn_18_101F4(void);
void fn_18_101F8(void);
void fn_18_101FC(void);
void fn_18_10200(void);
void fn_18_10204(struct Camera *arg0, struct Ball *arg1);
void fn_18_102C0(struct Camera *arg0, struct Ball *arg1);
void fn_18_1036C(void);
void fn_18_10370(struct Camera *arg0, struct Ball *arg1);
void fn_18_103DC(struct Camera *arg0);
void fn_18_10558(struct Camera *arg0);
void fn_18_10594(struct Camera *arg0);
void fn_18_105D0(struct Camera *arg0);
void fn_18_1060C(struct Camera *arg0);
void fn_18_10648(struct Camera *arg0);
void fn_18_10684(struct Camera *arg0);
void fn_18_106C0(struct Camera *arg0);
void fn_18_108B0(void);
void fn_18_108B4(void);
void fn_18_108B8(struct Camera *arg0);
void fn_18_10A14(void);
void fn_18_10A18(void);
void fn_18_10A1C(struct Camera *arg0);
void fn_18_10BFC(void);
void fn_18_10C00(void);
void fn_18_10C04(struct Camera *arg0);
void fn_18_10FE8(struct BBCamMove *arg0, struct Camera *arg1, Vec *arg2, S16Vec *arg3);
void fn_18_11094(struct BBCamMove *arg0, s32 arg1);
void fn_18_11190(struct BBCamMove *arg0, float farg0, float farg1, s16 arg1);
void fn_18_11380(struct BBCamMove *arg0, struct Camera *arg1, struct Ball *arg2);
void fn_18_1143C(s32 arg0);
void fn_18_1147C(s32 arg0, float farg0, float farg1, float farg2, float farg3);
void fn_18_11584(void);
void fn_18_1179C(void);
void fn_18_11A84(void);
void fn_18_11C7C(u8 arg0);
s16 *fn_18_11E38(u8 arg0);;
void fn_18_11E54(struct BBTex *arg0, s16 *arg1, struct GMAModel *arg2);;
void fn_18_11EFC(struct BBTex *arg0, u16 arg1, u16 arg2);;
void fn_18_11F48(void);
struct BBLayer *fn_18_12048(struct GMAModel *arg0, u32 arg1);;
void fn_18_120B4(void);
void fn_18_1214C(void);
void fn_18_1232C(void);
void fn_18_12540(void);
void fn_18_125CC(void);
void fn_18_126A0(float farg0, struct StageBgModel *arg0, s32 arg1);;
void fn_18_129F0(Mtx arg0, struct StageBgModel *arg1, s32 arg2);;
void fn_18_12AD8(s32 arg0);
void g_baseball_init_seesaws(void);;
s32 fn_18_12E34(void);
void fn_18_12E3C(u32 arg0);
void fn_18_12E70(void);;
void fn_18_130F0(void);
void fn_18_13120(u8 arg0);
void fn_18_131B8(void);
void fn_18_134B0(void);
void fn_18_134EC(void);
void fn_18_135D0(void);
void fn_18_13664(void);
void fn_18_138C0(void);
void fn_18_138F8(void);
void fn_18_13AE4(void);
void fn_18_13B48(void);
void fn_18_13CD4(void);
s32 fn_18_13CF4(Vec *arg0, Vec *arg1, Vec *arg2);
s32 fn_18_14008(s16 arg0, struct BBTrack **arg1, struct BBTrack **arg2);
void fn_18_14174(s32 arg0, float arg1);
s32 fn_18_141F8(void);
void fn_18_14224(void);
void fn_18_14288(void);
void fn_18_142C8(void);
void fn_18_142E0(void);
void fn_18_14308(void);
s32 fn_18_14394(void);
s32 fn_18_143E0(void);
void fn_18_14470(void);
void fn_18_1454C(void);
s32 fn_18_14608(void);
s32 fn_18_14654(void);
void fn_18_146E4(void);
void fn_18_147C8(void);
void fn_18_148F0(void);
s32 fn_18_14A20(void);
void fn_18_14A4C(void);
u8 fn_18_14AF4(void);;
void fn_18_14BB0(void);
s32 fn_18_14C78(void);
s32 fn_18_14D78(void);
void fn_18_14EAC(void);
void fn_18_150C0(void);
void fn_18_151B8(struct Ball *arg0);
void fn_18_15200(void);
void fn_18_15218(struct Ball *arg0, void *arg1);
void fn_18_153EC(void);
void fn_18_1542C(u8 arg0);
void fn_18_154A8(struct BBHelmet *arg0);
void fn_18_15664(struct BBTrack *arg0, float *arg1, float farg0);
void fn_18_157A8(void);
void fn_18_157C4(void);
void fn_18_15A08(void);
void fn_18_15A38(void);
void fn_18_15A88(void);
void fn_18_15AD0(void);
void fn_18_15E38(void);
void fn_18_15E60(void);
void fn_18_15E88(s16 arg0);
s32 fn_18_15EBC(s16 arg0);
void fn_18_15EE8(void);
void fn_18_15F2C(void);
void fn_18_16158(void);
void fn_18_163FC(void);
void fn_18_165B8(void);
void fn_18_166E8(u8 arg0);
void fn_18_16740(void);
void fn_18_1687C(float farg0);
void fn_18_16A94(void);
s16 fn_18_16B1C(struct PhysicsBall *arg0);
s32 fn_18_16B94(u8 arg0, s16 arg1);
void fn_18_16C48(struct PhysicsBall *arg0, struct BBWall *arg1);
s32 fn_18_1722C(struct PhysicsBall *a, struct PhysicsBall *b, float f);
void fn_18_17410(Vec *arg0, Vec *arg1, float farg0, Vec *arg2, float farg1);
s32 fn_18_175A4(Vec *arg0, struct PhysicsBall *arg1);
s32 fn_18_176BC(Vec *arg0);
s32 fn_18_17968(Vec *arg0);
s32 fn_18_17B7C(struct PhysicsBall *arg0, Mtx arg1);
u8 fn_18_17E40(void);
void fn_18_17F08(void);
void fn_18_17F7C(void);
void fn_18_1806C(void);
void fn_18_180DC(void);
void fn_18_18110(void);
s32 fn_18_18198(void);
s32 fn_18_1833C(void);
void fn_18_18364(void);
void fn_18_185AC(void);
void fn_18_186DC(void);
void fn_18_18750(void);
void fn_18_187C4(void);
void fn_18_18838(void);
s32 fn_18_1888C(void);
void fn_18_188AC(void);
void fn_18_18924(void);
void fn_18_18968(void (*arg0)());
u8 fn_18_18990(u8 arg0);
u8 fn_18_18A70(void);
s16 fn_18_18B00(void);
s32 fn_18_18B2C(void);
u8 fn_18_18B58(void);
u8 fn_18_18B68(void);
s32 fn_18_18B78(void);
s32 fn_18_18B88(void);
void fn_18_18BCC(void);
void fn_18_18DA4(void);
void fn_18_18DF0(void);
void fn_18_18E10(void);
u32 fn_18_18E30(u16 arg0);
void fn_18_18E50(u32 arg0);
void fn_18_18E70(u32 arg0, u8 arg1);
u32 fn_18_18E98(u32 arg0);;
void fn_18_18EB8(u16 arg0, Vec *arg1);
void fn_18_18ED8(u16 arg0);
void fn_18_18FD8(u16 arg0);
void fn_18_190D8(void);
void fn_18_1914C(u16 arg0);
void fn_18_19184(void);
void fn_18_191A4(u8 arg0);;
void fn_18_191C4(u8 arg0, s32 arg1, s32 arg2);
void fn_18_19230(void);
void fn_18_19260(s32 arg0);
void fn_18_192B0(void);
void fn_18_19318(void);
s32 fn_18_193C8(void);
void fn_18_19404(void);
s32 fn_18_1942C(void);
s32 fn_18_19468(void);
void fn_18_194B4(void);
s32 fn_18_19588(u8 arg0);
void fn_18_196A4(void);
void fn_18_1970C(void);
void fn_18_19730(void);
void fn_18_19798(void);
void fn_18_197EC(void);
void fn_18_19814(void);
u8 fn_18_1988C(void);
u8 fn_18_198B4(void);
u8 fn_18_198C8(void);
u8 fn_18_198DC(void);
u8 fn_18_19900(void);
u8 fn_18_19914(void);
u8 fn_18_19928(void);
u8 fn_18_1993C(void);
u8 fn_18_19950(void);
u8 fn_18_19964(void);
u8 fn_18_19978(void);
u8 fn_18_199A0(u8 arg0, u8 arg1);
u8 fn_18_199EC(void);;
u8 fn_18_19A34(void);
u8 fn_18_19A7C(void);;
u8 fn_18_19AC4(void);
u8 fn_18_19B0C(void);
u8 fn_18_19B34(void);
void fn_18_19B48(void);
s32 fn_18_19B68(void);
s32 fn_18_19B90(void);
s32 fn_18_19BB8(void);
s32 fn_18_19BE0(void);
void fn_18_19C08(void);
void fn_18_19D98(void);
void fn_18_19D9C(void);
void fn_18_1A040(float farg0, float farg1);
void fn_18_1A35C(void);
s32 fn_18_1AD98(void);
void fn_18_1B6C4(void);
s32 fn_18_1C128(void);
void fn_18_1C6AC(void);
s32 fn_18_1CA08(void);
void fn_18_1CF8C(void);
void fn_18_1D480(void);
void fn_18_1D6C4(void *arg0);
s32 g_get_sprite_work(struct BBScreen *arg0, u32 arg1);
void fn_18_1F400(struct BBScreen *arg0, s32 arg1);
struct BBScreen *fn_18_21768(s32 arg0);
s32 fn_18_21854(struct BBScreen *arg0);
u32 fn_18_218EC(s32 arg0);
u32 fn_18_21918(s32 arg0, Vec *arg1);
void fn_18_21A34(s32 arg0);
void fn_18_21A68(void);
s32 fn_18_21AA8(char *arg0, s32 arg1);
void fn_18_21B4C(u8 arg0);
void fn_18_21BB8(u8 arg0, s32 arg1);
void fn_18_21BE4(void);
void fn_18_21E6C(char *arg0);
void fn_18_22268(void);
void fn_18_2227C(void);
void fn_18_2233C(void);
void fn_18_2239C(void);
void fn_18_223C4(Vec *arg0);
void fn_18_223E4(Vec *arg0, Vec *arg1, float farg0);
void fn_18_22414(Vec *arg0, Vec *arg1);
void fn_18_22434(Vec *arg0);
void fn_18_22454(Vec *arg0);
void fn_18_22458(void);
void fn_18_2248C(Vec *arg0, Vec *arg1, Vec *arg2);
void fn_18_224AC(void);
void fn_18_22664(void);
void fn_18_227FC(void);
void fn_18_22834(void);
void fn_18_22AC4(s8 arg0, s8 arg1, s8 arg2, s8 arg3);
void fn_18_22AE4(u8 arg0, u8 arg1);
void fn_18_22B00(void);
void fn_18_22CE0(void);
void fn_18_22E7C(s8 *arg0, struct Sprite *arg1);
void fn_18_23BC8(s32 arg0, struct Sprite *arg1, struct SpriteDrawRequest *arg2);
void fn_18_24164(s8 arg0, struct Sprite *arg1, struct SpriteDrawRequest *arg2, s8 arg3, s8 arg4, s8 arg5, s8 arg6);
void fn_18_2489C(struct Sprite *arg0);
void fn_18_25188(struct Sprite *arg0);
void fn_18_251AC(void);
void fn_18_25298(void);
void fn_18_252CC(struct Sprite *sprite);
void fn_18_255D4(struct Sprite *sprite);
void fn_18_25A80(void);
s32 fn_18_25AB8(void);
void fn_18_260BC(void);
void fn_18_26104(void);
void fn_18_2627C(void);
void fn_18_26368(struct BBCpu *cpu, u8 arg1);
void fn_18_263E4(struct BBCpu *cpu);
void fn_18_26400(void);
void fn_18_26440(void);
void fn_18_26458(struct BBCpu *cpu);
void fn_18_26504(struct BBCpu *cpu);
void fn_18_26564(struct BBCpu *cpu);
void fn_18_2675C(struct BBCpu *cpu);
void fn_18_26800(struct BBCpu *cpu);
void fn_18_26878(struct BBCpu *cpu);
void fn_18_268BC(struct BBCpu *cpu);
void fn_18_26A6C(struct BBCpu *cpu);
void fn_18_26D20(struct BBCpu *cpu);
void fn_18_26EC0(struct BBCpu *cpu);
void fn_18_27118(u8 arg0);
void fn_18_27148(u8 arg0);
void fn_18_2717C(s16 *arg0);
void fn_18_271A4(s16 *arg0, s16 arg1, s16 arg2, s16 arg3);
void fn_18_27234(void);
void fn_18_272F0(void);
void fn_18_274E0(s8 *arg0, s8 *arg1, u16 *arg2);
void fn_18_275A0(void);
void fn_18_275D4(struct BBCpu *cpu);
void fn_18_277CC(struct BBCpu *cpu);
void fn_18_27968(struct BBCpu *cpu);

#endif
