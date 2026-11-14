#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "background.h"
#include "bitmap.h"
#include "camera.h"
#include "file.h"
#include "gx.h"
#include "input.h"
#include "event.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "pool.h"
#include "recplay.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "world.h"

#define PAD pad_status_groups->unk0
#define PAD_REPEAT(b)                                                                              \
    ((PAD[4].button & (b)) || (analog_inputs[0][4] & (b))                                          \
     || (((PAD[0].button & (b)) || (analog_inputs[0][0] & (b))) && (analog_inputs[0][0] & 0x200)))

#define PAD_ANA(b)                                                                                 \
    ((PAD[4].button & (b)) || ((PAD[0].button & (b)) && (analog_inputs[0][0] & 0x200)))

#define PAD_NAV(b)                                                                                 \
    ((PAD[2].button & (b)) || (PAD[4].button & (b))                                                \
     || ((PAD[0].button & (b)) && (PAD[0].button & 0x400)))

struct ReverbHI
{
    float v[6];
};

struct Chorus
{
    u32 v[3];
};

struct MenuEntry
{
    /*0x00*/ int type;
    /*0x04*/ s16 x;
    /*0x06*/ s16 y;
    /*0x08*/ char *en;
    /*0x0C*/ char *jp;
    /*0x10*/ int subMode;
};  /* size = 0x14 */

struct MenuPage
{
    /*0x00*/ struct MenuEntry *entries;
    /*0x04*/ int count;
};

struct EffectParam
{
    /*0x00*/ char *name;
    /*0x04*/ float step;
    /*0x08*/ float min;
    /*0x0C*/ float max;
};

struct Effect
{
    /*0x00*/ struct EffectParam *params;
    /*0x04*/ u8 count;
    /*0x05*/ u8 id;
    /*0x06*/ u8 pad6;
    /*0x07*/ u8 pad7;
    /*0x08*/ char *name;
};

struct TexFormat
{
    /*0x00*/ int format;
    /*0x04*/ char *name;
};

struct LoadSet
{
    /*0x00*/ char *dir;
    /*0x04*/ char *tpl;
    /*0x08*/ char *gma;
};

struct BmpField
{
    /*0x00*/ int type;
    /*0x04*/ char *fmt;
    /*0x08*/ void *value;
};

struct FontSample
{
    /*0x00*/ int font;
    /*0x04*/ char *text;
};

struct FontGroup
{
    /*0x00*/ struct FontSample *samples;
    /*0x04*/ char *name;
    /*0x08*/ int unk8;
};

struct ModelParam
{
    /*0x00*/ int id;
    /*0x04*/ float a;
    /*0x08*/ float b;
    /*0x0C*/ float c;
    /*0x10*/ int d;
    /*0x14*/ int e;
};

struct ShadowDebugQuad
{
    /*0x00*/ float z;
    /*0x04*/ float x;
    /*0x08*/ float y;
    /*0x0C*/ float x2;
    /*0x10*/ float y2;
    /*0x14*/ int flag;
    /*0x18*/ u8 color[4];
    u8 filler1C[0x50 - 0x1C];
};

struct ModelEntry
{
    /*0x00*/ int type;
    /*0x04*/ char *dir;
    /*0x08*/ char *name;
};

struct RomFont
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ float unk4;
};

struct PreRender
{
    /*0x00*/ char *en;
    /*0x04*/ char *jp;
    /*0x08*/ u16 w;
    /*0x0A*/ u16 h;
    /*0x0C*/ int unkC;
    /*0x10*/ int unk10;
    /*0x14*/ int unk14;
};

struct Dummy5ABC
{
    /*0x00*/ char *name;
    /*0x04*/ u16 *cleared;
    /*0x08*/ u16 *failed;
    /*0x0C*/ s16 count;
    /*0x0E*/ s16 rows;
    /*0x10*/ s8 first;
    /*0x11*/ u8 pad[3];
};

struct ReplaySize
{
    /*0x00*/ s16 mode;
    /*0x02*/ s16 next;
    /*0x04*/ s16 count;
    u8 unk6[2];
    /*0x08*/ u32 minSize;
    /*0x0C*/ u32 maxSize;
    /*0x10*/ s16 minStage;
    /*0x12*/ s16 maxStage;
};

struct PassRate
{
    /*0x0*/ s16 scroll;
    /*0x2*/ s16 mode;
};

struct TexCopy
{
    /*0x00*/ int fmt;
    /*0x04*/ int alpha;
    /*0x08*/ GXTexObj tex;
    /*0x28*/ void *buf;
};

struct NlTest
{
    /*0x00*/ int model;
    /*0x04*/ int set;
    /*0x08*/ struct NlObj *obj;
    /*0x0C*/ struct TPL *tpl;
};

struct BmpTest
{
    /*0x00*/ int category;
    /*0x04*/ int id;
    /*0x08*/ int row;
    /*0x0C*/ u8 baseR, baseG, baseB;
    /*0x0F*/ u8 ofsR, ofsG, ofsB;
    /*0x12*/ s16 zang;
    /*0x14*/ float trnsl;
    /*0x18*/ float zoomX, zoomY;
    /*0x20*/ float step;
    /*0x24*/ float u0, v0, u1, v1;
};

struct SkelName { u8 pad0[0x14]; char *name; };
struct ModelName { char *name; u8 pad4[0x18]; };
struct MotLists { struct SkelName *skels; u32 skelCount; struct ModelName *models; u32 modelCount; };
struct VtxPosNrm { Vec pos; Vec nrm; u8 pad18[0x28]; };

struct PatTest
{
    /*0x00*/ struct TPL *tpl;
    /*0x04*/ struct GMA *gma;
    /*0x08*/ int state;
    /*0x0C*/ int category;
    /*0x10*/ int groupCount;
    /*0x14*/ int group;
    /*0x18*/ int speed;
    /*0x1C*/ int *starts;
    /*0x20*/ int *ends;
    /*0x24*/ char **names;
};

struct DialogBox
{
    u8 unk0[0x9C];
};

struct DialogTest
{
    /*0x000*/ s16 category;
    /*0x002*/ s16 index;
    /*0x004*/ s16 box;
    /*0x006*/ s16 ape;
    /*0x008*/ int *tbl;
    /*0x00C*/ struct DialogBox boxes[3];
    /*0x1E0*/ struct Ape *apes[6];
};

struct ReplayTest
{
    /*0x000*/ int id;
    /*0x004*/ int state;
    /*0x008*/ char path[0x100];
};
extern s32 dip_switches;
extern s16 g_another_stage_id;
extern s32 g_debug_sound_aram_usage;
extern s32 g_debug_sound_ram_usage;
extern s32 g_player_id_for_sound;
extern s32 g_some_other_flags;
extern s32 g_some_render_flag;
extern s16 g_some_stage_id;
extern s32 * graphics_info;
extern int lbl_1_bss_16040;
extern int lbl_1_bss_16084;
extern s32 lbl_1_bss_24F64;
extern s32 lbl_1_bss_6FB68;
extern s32 lbl_1_bss_6FB6C;
extern s32 lbl_1_bss_C8B48;
extern s8 s_lightPool;

extern void (*g_some_func_ptr_related_to_sub_mode2)(void);
extern void (*test_draw_func_ptr)(void);
extern u8 lbl_1_bss_D1320[];
void dest_all_events(void);
void SoundPan(u16 soundId, s8 pan, s8 span);
void SoundPitch(u16 soundId, u16 pitch);
void g_something_with_GX_vtx_desc(u32 attrs);
void opti_GXSetChanCtrl(int chan, int enable, int ambSrc, int matSrc, int lightMask, int diffFn, int attnFn);
void g_GXSetTevIndirect_zero_if_different(int stage);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevOrder_wrapper(int stage, int texCoord, int texMap, int color);
void g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(int stage, int mode);
void GXSetNumTevStages_cached(u8 a);
void GXSetNumTexGens_cached(u8 a);
void GXSetNumChans_cached(int a);
void GXSetNumIndStages_cached(u8 n);
void mtxa_to_quat(Quat *q);
void PSVECAdd(Vec *a, Vec *b, Vec *out);
void SoundDop(u16 soundId, u16 pitch);
int *fn_8006991C(void *model);
struct VtxPosNrm *fn_80069870(void *model);
void GXInitTexObjLOD(GXTexObj *obj, int minFilter, int magFilter, float minLod, float maxLod, float lodBias, u8 biasClamp, u8 doEdgeLod, int maxAniso);
u32 *OSGetStackPointer(void);
void SoundGroupLoad(int group);
void SoundIcsReq(u32 soundId, u8 intensity, s8 volume);
void fade_screen_to_color(s32 type, u32 color, int duration);
void fn_1_56C18(void);
void fn_1_D1F28(struct Ape *ape);
void fn_1_D2510(struct Ape *ape);
void fn_1_D29E0(int id);
void fn_1_D4870(void);
void g_draw_bg(void);
void g_maybe_some_bg_effect_handler(void);
void g_set_up_debugtext_buffer(void);
void g_something_calls_sndPopGroup(void);
void g_very_similar_to_puts(const char *s);
void handle_test_camera(void);
void load_bmp_by_id(int id);
void load_stage(int stageId);
void ord_tbl_draw_nodes(void);
void reset_ui_widescreen_scale_mtx(void);
void set_ui_widescreen_scale_mtx(int width);
void textdraw_clear_flags(u32 flags);
void textdraw_printf(const char *fmt, ...);
void textdraw_put_char(u8 c);
void textdraw_reset(void);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float scaleX, float scaleY);
int DVDChangeDir(const char *);
void event_dest(int id);
void event_init(int id);
void textdraw_print(const char *str);
struct Ape *g_load_ape_gameplay_variant0(int a, int lod);
void draw_ape(struct Ape *ape);
BOOL is_bonus_stage(int stageId);
void GXInitTexObj(GXTexObj *obj, void *image, u16 width, u16 height, int format, int wrapS, int wrapT, int mipmap);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, u8 mipmap, u8 maxLod);
void GXSetTexCopySrc_cached(u16 left, u16 top, u16 width, u16 height);
void GXSetTexCopyDst_cached(u16 width, u16 height, int format, u8 mipmap);
void GXCopyTex(void *dest, int clear);
void fn_1_E2F1C(void);
void textdraw_set_spacing(float x, float y);
void textdraw_set_alignment(int alignment);
void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
int DVDOpen(char *file, struct DiscFileInfo *info);
int DVDClose(struct DiscFileInfo *info);
void fn_1_D462C(void);
void debug_textdraw_set_pos(int x, int y);
void GXGetProjectionv(float *proj);
void debug_textdraw_set_color(u8 color);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(const char *fmt, ...);
void fn_1_3E100(int a, int b);
BOOL g_is_sjis_and_more_than_24_mib(void);
void set_sub_mode_destination(void (*func)(void));
void fn_1_34828(void);
void fn_1_36140(void);
void C_MTXLookAt(Mtx m, Vec *camPos, Vec *camUp, Vec *target);
void GXInvalidateTexAll(void);
void g_set_something3(int arg0);
void g_draw_stage(void);
void g_draw_stobjs(void);
unsigned long strlen(const char *s);
void textdraw_set_font_style(s8 style);
void textdraw_set_drop_shadow(void);
void textdraw_set_add_color(u32 color);
int strncmp(const char *a, const char *b, unsigned long n);
void unload_stage(void);
void fn_1_1118BC(void);
void GXSetCopyClear_cached(GXColor color, u32 z);
void perf_init_timer(int timer);
void fn_1_E2D20(void);
void fn_1_73900(int stage);
u32 fn_1_363C8(int track);
s16 g_maybe_related_to_music_crossfading(int track);
void g_change_music_volume(int track, u32 time, s8 volume);
void call_SoftStreamSEReq_arg_0(int id, s8 volume, s8 pan, s8 span);
void fn_1_34B48(s16 id, s8 volume);
void fn_1_D1FB8(struct Ape *ape, int b);
void call_free_bmp_by_id(int grpId);
void fn_1_D3930(void);
void fn_1_141288(void);
void load_scene_files(int category);
void fn_1_142ECC(void);
void g_load_efcmdl_gmatpl(void);
void get_load_group_status(void);
void fn_1_141370(u16 sceneId, u16 frame);
void g_something_freeing_heap_4(void);
void fn_1_141E48(void);
void fn_1_142F00(void);
void g_something_with_cutscenes_func2(void);
void g_something_freeing_heap_parent(void);
void fn_1_1439F4(void);
void fn_1_81E4(GXTexObj *obj, struct ShadowDebugQuad *quad);
void *load_dialog_tbl(int a);
void fn_1_162978(void *tbl);
void fn_1_1607F4(struct DialogBox *box, int a);
void fn_1_15FB34(struct DialogBox *box, int a);
void fn_1_15FB70(struct DialogBox *box);
void fn_1_15FB94(struct DialogBox *box);
void fn_1_160518(struct DialogBox *box, void *data);
void fn_1_1607DC(struct DialogBox *box, struct Ape *ape);
void fn_1_1607E4(struct DialogBox *box);
void fn_1_160870(struct DialogBox *box);
void fn_1_143798(int stage);
void GXSetChanAmbColor(int chan, GXColor color);
void GXSetChanMatColor(int chan, GXColor color);
BOOL load_nlobj(struct NlObj **nlObj, struct TPL **tpl, char *nlobjPath, char *tplPath);
void fn_1_618F4(int replay);
void g_advance_stage_animation(void);
void get_curr_stage_fly_in_position(struct Sphere *sphere);
char *strncpy(char *dst, const char *src, unsigned long n);
void fn_1_5FE5C(int id);
void avdisp_set_fog_params(int type, float start, float end);
void avdisp_set_fog_color(int r, int g, int b);
void draw_shadow_reqs(void);
void call_g_avdisp_set_ambient(float r, float g, float b);
void *fn_1_6333C(int replayId);
u32 fn_1_63354(void *replay);
void fn_1_6335C(void *replay, void *buf);
void *fn_8006F330(char *path, char *mode);
void fclose(void *file);
u32 fwrite(void *buf, u32 size, u32 count, void *fp);
u32 get_gx_fifo_use_size(void);
s16 play_track_and_fade_out_other_tracks(int track, int fadeTime, s8 volume);
void fn_1_F9E1C(void *tex);
void fn_1_F9EAC(void *tex, float a, float b, float c, float d, float e, int f, int g);
u32 perf_stop_timer(int timer);
void draw_items(void);
void g_draw_effects(void);
void GXDrawSphere(u8 numMajor, u8 numMinor);
void game_main_draw_func(void);
void init_shadow_reqs(void);
void g_load_sprite_tex_from_dvd(struct SpriteTexture *tex);
void fn_1_F9CAC(void *preview, int index);
s8 fn_1_36534(int a);
char *fn_1_36488(int a);
void g_something_with_stopping_music_or_sfx(int track);
s16 fn_1_34A28(int id);
void g_stop_music_sound_debug(int id);
char *fn_1_34AB0(int id);
s16 g_another_SoftStreamStart_wrapper_of_some_sort(int track, s8 volume);
void GXSetCurrentMtx(int id);
void GXSetVtxAttrFmt(int fmt, int attr, int cnt, int type, u8 frac);
void event_sound_dest(void);
void fn_1_D292C(void);
void dest_all_sprites(void);
void textdraw_set_drop_shadow_with_params(int x, int y, float a);
void u_reset_light_group_stack(int unused);
void g_load_preview_texture(void *tex, char *file, int id, int w, int h, int fmt);
void light_init(int stageId);
void SoundRev(u16 id, u8 val);
void SoundCho(u16 id, u8 val);
int fn_1_D288C(void (*func)(struct Ape *, int), struct Ape *arg, int list);
BOOL free_nlobj(struct NlObj **nlObj, struct TPL **tpl);
void nl2ngc_draw_model_sort_translucent(struct NlModel *model);
void init_course(void);
int fn_1_A3C84(int difficulty, int floor, u32 flags);
void g_GX_something(GXTexObj *, int *, int *, float *, float *, float *, u8 *, u8 *, int *);
int printf(const char *fmt, ...);
void OSPanic(const char *file, int line, const char *fmt, ...);
void SoundMod(u16 id, u16 val);
void fn_1_34C08(s16 id, s8 pan, s8 span);
struct ReverbHI fn_1_36E18(void);
struct Chorus fn_1_36E7C(void);
void g_snd_dbg_call_sndAuxCallbackupdateSettingsReverbHI_1(struct ReverbHI *);
void g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_2(struct Chorus *);
void textdraw_set_pos_for_aram_font_test(float, float);
char *fn_1_34AFC(int);
char *fn_1_364E8(int);
s8 fn_1_36550(int);
void *memcpy_handler(void *dst, const void *src, unsigned long n);
extern char **bmp_tex_names[];
extern char *FONT_NAME_LIST[];
s32 g_get_font_def_aram_flag(s32);
extern int g_related_to_aram_font_pages[];
extern char *FONT_STYLE_NAME_LIST[];
void fn_1_D1F0C(int, int);
struct Ape *fn_1_D1F48(char *, char *);
void fn_1_D1FBC(struct Ape *, struct ModelParam *, int);
extern struct MotLists *lbl_1_bss_C8AE8;
struct Mot700B0 { s16 unk0; u16 unk2; u8 pad[0x14]; };
extern struct Mot700B0 *lbl_1_bss_700B0;
extern s32 lbl_1_bss_C8AEC;
void fn_80065BA8(void *);
void *g_load_raw_gma(char *, struct TPL *);
void fn_800699B4(void *, int, int);
int sprintf(char *buf, const char *fmt, ...);
extern Mtx **g_some_buffer_ptr;
void GXGetTexObjAll(GXTexObj *obj, u8 **image, u16 *w, u16 *h, int *format, int *wrapS, int *wrapT, u8 *mipmap);
void *memset(void *dst, int c, unsigned long n);
void GXSetProjectionv(float *proj);
int g_something_with_cutscenes_func(u16 sceneId);
void g_dump_scene_info(int sceneId);
extern s32 lbl_1_bss_DD4EC;
extern int g_active_scene_category;
char *fn_1_14319C(u16 sceneId);
int fn_1_143104(u16 sceneId);
extern u32 g_font_char_var_3;
void *g_smth_with_dialog2(void *tbl, int i);
extern char *lbl_1_data_70D98[];
extern char *lbl_1_data_2D964[];
extern char *lbl_1_data_2DD58[];
extern u32 lbl_1_bss_D2B7C;
void fn_1_F0AEC(int a, int b, int c);
void g_some_printf_function_6(int a, s16 b, int c, ...);
int fn_1_664EC(void);
void GXSetPixelFmt(int, int);
void VISetNextFrameBuffer(s32);
void VIWaitForRetrace();
void fn_1_365AC(s16, s8);
void fn_1_F0E4C(int, int);
void fn_8_10070();
void fn_8_100FC();
void fn_8_10464();
void fn_8_1226C();
void fn_8_123A8();
void fn_8_12404();
void fn_8_12534();
void fn_8_12F1C();
void fn_8_1302C();
void fn_8_13080();
void fn_8_130A4();
void fn_8_165C4();
s32 fn_8_16B04();
void fn_8_16E24();
void fn_8_16EEC();
void fn_8_16F18();
void fn_8_171C4();
void fn_8_175C0();
void fn_8_175E0();
void fn_8_177C4();
void fn_8_1783C();
void fn_8_179A0();
void fn_8_FDF0();
void fn_8_FFC4();
void g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_1();
void g_snd_dbg_call_sndAuxCallbackUpdateSettingsReverbHI_2();

void fn_8_144(void);
void fn_8_1D4(void);
void fn_8_280(void);
void fn_8_770(void);
int fn_8_9FC(int type);
void fn_8_A18(int n);
void fn_8_B1C(void);
void fn_8_B74(void);
void fn_8_E40(void);
void fn_8_E64(void);
void fn_8_15FC(void);
void fn_8_171C(void);
void fn_8_2AB4(void);
void fn_8_336C(void *rev, void *cho);
void fn_8_33FC(void);
void fn_8_3434(void);
void fn_8_3454(void);
void fn_8_3594(void);
void fn_8_36E4(void);
void fn_8_3740(void);
void fn_8_3AD0(void);
void fn_8_3B18(void);
void fn_8_3B38(void);
void fn_8_3B70(void);
void fn_8_3B90(void);
void fn_8_3C4C(void);
void fn_8_3C6C(void);
void fn_8_3D84(void);
void fn_8_3DBC(void);
void fn_8_40F8(void);
void fn_8_4254(void);
void fn_8_4D5C(void);
void fn_8_52F4(void);
void fn_8_5340(void);
void fn_8_5740(void);
void fn_8_5A14(void);
void fn_8_5A78(void);
void fn_8_5EF0(void);
void fn_8_64D4(struct Ape *arg0, s32 arg1);
void fn_8_6678(void);
void fn_8_6AD4(void);
void fn_8_7608(void);
void fn_8_7638(void);
void fn_8_7658(void);
void fn_8_767C(void);
void fn_8_76FC(void);
void fn_8_7820(void);
void fn_8_7A0C(void);
void fn_8_7CC8(void);
void fn_8_7DDC(void);
void fn_8_89B4(void);
void fn_8_8D74(void);
void fn_8_8E60(void);
void fn_8_9074(void);
void fn_8_9C44(void);
void fn_8_9FA4(struct GMAModel *model);
void fn_8_A0E4(struct GMAModel *model);
void fn_8_A82C(int arg0);
void fn_8_ABE4(void);
void fn_8_ACFC(void);
void fn_8_B14C(void);
void fn_8_B3C4(void);
void fn_8_B818(void);
void fn_8_B864(void);
void fn_8_B994(void);
void fn_8_BB78(void);
void fn_8_C2A0(void);
void fn_8_C314(void);
void fn_8_C74C(float width, float height);
void fn_8_C8F8(s8 *status, struct Sprite *sprite);
void fn_8_C91C(struct Sprite *arg0);
void fn_8_C960(struct Sprite *arg0);
void fn_8_C984(void);
void fn_8_C9EC(void);
void fn_8_CEB4(void);
void fn_8_CFDC(void);
void fn_8_D97C(void);
void fn_8_DA2C(void);
void fn_8_E088(void);
void fn_8_E100(void);
void fn_8_E428(void);
void fn_8_E674(void);
void fn_8_EA44(void);
void fn_8_EB10(void);
void fn_8_ED34(void);
void fn_8_ED74(void);
void fn_8_EE5C(void);
void fn_8_F140(void);
void fn_8_F1AC(void);
void fn_8_F6B8(void);

static s16 b0;
static s16 b2;
static int b4;
static int b8;
static int bC;
static int b10;
static int b14;
static s8 b18;
static s8 b19;
static s8 b1A;
static u16 b1C;
static u16 b1E;
static u16 b20;
static u8 b22;
static u8 b23;
static s8 b24;
static s8 b25;
static s8 b26;
static s16 b28;
static s8 b2A;
static s8 b2B;
static s8 b2C;
static s16 b2E;
static s8 b30;
static s8 b31;
static int b34;
static s8 b38;
static s8 b39;
static struct TexCopy s_copy;
static struct NlTest s_nl;
static struct BmpTest s_bmp;
static int bAC;
static int bB0;
static int bB4;
static s16 bB8;
static s16 bBA;
static s8 bBC;
static s8 bBD;
static s8 bBE;
static int bC0;
static int bC4;
static struct Ape *bC8[0x10];
static int b108;
static int b10C;
static int b110;
static int b114[0x10];
static Vec b154;
static float b160;
static void *b164;
static struct TPL *b168;
static Vec *b16C;
static int *b170;
static struct VtxPosNrm *b174;
static int b178;
static int b17C[5];
static s16 b190;
static struct TPL *b194;
static struct GMA *s_gma[2];
#define b198 s_gma[0]
static int b1A0;
static s16 b1A4;
static int b1A8;
static s16 b1AC;
static s16 b1AE;
static s16 s_sel[3];
#define b1B0 s_sel[0]
static s16 b1B6;
static u8 b1B8;
static float b1BC;
static void *b1C0[0x100];
static int b5C0;
static s16 b5C4[3];
static u8 b5CA;
static u8 b5CB;
static float b5CC[3];
static s16 b5D8;
static float b5DC;
static s16 b5E0;
struct PatTest *b5E4;
static u16 b5E8;
static s16 b5EA;
static int b5EC;
static void *b5F0;
static u8 b5F4[0x78];
static struct ReplayTest s_rep;
static s16 b774;
static s16 b776;
static s16 b778;
static s16 b77A;
static u8 b77C;
#define b66C s_rep.id
#define b670 s_rep.state
static struct DialogTest s_dlg;
static struct PassRate s_pass;
static struct ReplaySize s_size;
static u8 s_sizePad[0x7c0];

static u8 D0[0x170] = {
    254, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 254, 13,
    14, 15, 255, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    12, 13, 14, 15, 255, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0,
    56, 0, 180, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0,
    16, 0, 76, 0, 0, 3, 0, 0, 1, 0, 0, 0, 0, 4, 0, 0,
    24, 0, 0, 0, 0, 9, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 11, 0, 0, 24, 0, 0, 0, 0, 12, 0, 0,
    24, 0, 0, 0, 0, 5, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 7, 0, 0, 24, 0, 0, 0, 0, 8, 0, 0,
    56, 0, 180, 0, 0, 13, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0,
    24, 0, 0, 0, 0, 20, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 22, 0, 0, 24, 0, 0, 0, 0, 23, 0, 0,
    24, 0, 0, 0, 0, 24, 0, 0, 24, 0, 0, 0, 0, 15, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 17, 0, 0,
    24, 0, 0, 0, 0, 18, 0, 0, 24, 0, 0, 0, 0, 19, 0, 0,
    24, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 1, 0, 0,
    24, 0, 0, 0, 0, 2, 0, 0, 24, 0, 0, 0, 0, 3, 0, 0,
    24, 0, 0, 0, 0, 4, 0, 0, 24, 0, 0, 0, 0, 5, 0, 0,
    24, 0, 0, 0, 0, 6, 0, 0, 24, 0, 0, 0, 0, 7, 0, 0,
    24, 0, 0, 0, 0, 8, 0, 0, 24, 0, 0, 0, 0, 9, 0, 0,
    24, 0, 0, 0, 0, 10, 0, 0, 24, 0, 0, 0, 0, 11, 0, 0,
    24, 0, 0, 0, 0, 12, 0, 0, 24, 0, 0, 0, 0, 13, 0, 0,
    24, 0, 0, 0, 0, 14, 0, 0, 24, 0, 0, 0, 0, 15, 0, 0,
};

static void (*D170[54])(void) = {
    fn_8_1D4, fn_8_280, fn_8_E40, fn_8_E64,
    fn_8_B1C, fn_8_B74, fn_8_15FC, fn_8_171C,
    fn_8_3454, fn_8_3594, fn_8_3AD0, fn_8_3B18,
    fn_8_3B38, fn_8_3B70, fn_8_3B90, fn_8_3C4C,
    fn_8_3C6C, fn_8_3DBC, fn_8_40F8, fn_8_4254,
    fn_8_52F4, fn_8_5340, fn_8_CEB4, fn_8_CFDC,
    fn_8_6678, fn_8_6AD4, fn_8_7658, fn_8_76FC,
    fn_8_7820, fn_8_8E60, fn_8_9074, fn_8_ABE4,
    fn_8_ACFC, fn_8_B818, fn_8_B864, fn_8_B994,
    fn_8_BB78, fn_8_C984, fn_8_C9EC, fn_8_33FC,
    fn_8_3434, fn_8_7608, fn_8_7638, fn_8_5A14,
    fn_8_5A78, fn_8_D97C, fn_8_DA2C, fn_8_E428,
    fn_8_E674, fn_8_ED34, fn_8_ED74, fn_8_F140,
    fn_8_F1AC, NULL,
};

void _prolog(void)
{
    g_some_func_ptr_related_to_sub_mode2 = fn_8_144;
    test_draw_func_ptr = fn_8_F6B8;
    g_very_similar_to_puts("TEST MODE LOADED\n");
}

void _epilog(void)
{
    g_some_func_ptr_related_to_sub_mode2 = NULL;
    test_draw_func_ptr = NULL;
    g_very_similar_to_puts("TEST MODE PURGED\n");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    for (sp = OSGetStackPointer(); sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16; sp = (u32 *)*sp)
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
    OSPanic("test_mode.c", 0xAD, "\n");
}

void fn_8_144(void)
{
    if (sub_mode <= 0x71 || sub_mode >= 0xA7)
    {
        printf("sub_mode: error %d in Test", sub_mode);
        return;
    }
    {
        int i = sub_mode - 0x72;

        D170[i]();
    }
}

void fn_8_1D4(void)
{
    lbl_1_bss_C8B48 = 0;
    camera_setup_singleplayer_viewport();
    fn_8_123A8();
    fn_8_1302C();
    fn_8_FFC4();
    fn_8_36E4();
    fn_8_3D84();
    fn_8_175C0();
    fn_8_179A0();
    fade_screen_to_color(0, 0, 1);
    fn_1_34828();
    fn_1_36140();
    free_bmp_by_id(0x19);
    unload_stage();
    dest_all_events();
    light_init(0);
    b0 = 0;
    b2 = 0;
    sub_mode_request = 0x73;
}

static struct MenuEntry D46C[16] = {
    { 1, 6, 4, "STAGE NUM: ", "\212J\216n\203X\203e\201[\203W\201F ", 0 },
    { 2, 4, -2, "MODEL", "\203\202\203f\203\213", 0 },
    { 0, 6, -2, "AV MODEL", "\202f\202b\203\202\203f\203\213", 143 },
    { 0, 6, -2, "NAOMI MODEL", "\202m\202`\202n\202l\202h\203\202\203f\203\213", 130 },
    { 3, 6, -2, "EFFECTIVE", "\203G\203t\203F\203N\203e\203B\203u\203\202\203f\203\213", 141 },
    { 0, 6, -2, "PATTERN", "\203p\203^\201[\203\223\203`\203F\203\223\203W", 145 },
    { 2, 4, -2, "MOTION", "\203\202\201[\203V\203\207\203\223", 0 },
    { 0, 6, -2, "NEW MOTION", "\203\202\201[\203V\203\207\203\223", 155 },
    { 0, 6, -2, "SCENE PLAY", "\203V\201[\203\223\215\304\220\266", 159 },
    { 0, 6, -2, "DIALOG", "\203Z\203\212\203t", 161 },
    { 2, 4, -2, "ETC", "\202\273\202\314\221\274", 0 },
    { 0, 6, -2, "BLUR", "\203u\203\211\201[", 122 },
    { 0, 6, -2, "SHADOW", "\211e", 124 },
    { 0, 6, -2, "FOCUS", "\203t\203H\201[\203J\203X", 126 },
    { 0, 6, -2, "INDIRECT", "\203C\203\223\203_\203C\203\214\203N\203g", 128 },
    { 0, 6, -2, "DIP SWITCH", "\203f\203B\203b\203v\203X\203C\203b\203`", 118 },
};

static struct MenuEntry D6E4[16] = {
    { 2, 28, 4, "BITMAP", "\203r\203b\203g\203}\203b\203v", 0 },
    { 0, 30, -2, "BITMAP", "\203r\203b\203g\203}\203b\203v", 132 },
    { 0, 30, -2, "FONT", "\203t\203H\203\223\203g", 134 },
    { 0, 30, -2, "ARAM FONT", "\202`\202q\202`\202l\203t\203H\203\223\203g", 157 },
    { 0, 30, -2, "ROM FONT", "\202q\202n\202l\203t\203H\203\223\203g", 147 },
    { 0, 30, -2, "PRERENDER", "\203v\203\214\203r\203\205\201[\215\354\220\254", 149 },
    { 0, 30, -2, "PREVIEW", "\203v\203\214\203r\203\205\201[\225\\\216\246", 151 },
    { 2, 28, -2, "INPUT", "\223\374\227\315", 0 },
    { 0, 30, -2, "INPUT", "\223\374\227\315", 116 },
    { 2, 28, -2, "SOUND", "\203T\203E\203\223\203h", 0 },
    { 0, 30, -2, "SOUND", "\203T\203E\203\223\203h", 120 },
    { 0, 30, -2, "ADX", "\202`\202c\202w\203X\203g\203\212\201[\203~\203\223\203O", 153 },
    { 2, 28, -2, "REPLAY", "\203\212\203v\203\214\203C", 0 },
    { 0, 30, -2, "REPLAY", "\203\212\203v\203\214\203C", 136 },
    { 0, 30, -2, "REPLAY SIZE CHECKER", "\203\212\203v\203\214\203C \203T\203C\203Y\214\237\215\270", 165 },
    { 0, 30, -2, "DIFFICULTY", "\203f\203B\203t\203B\203J\203\213\203e\203B", 163 },
};

static struct MenuPage D824[2] = {
    { D46C, 16 },
    { D6E4, 16 },
};

void fn_8_280(void)
{
    struct MenuPage *page;
    int sel;
    struct MenuEntry *entry;
    int v4;

    if ((u32)(g_some_other_flags & 0xA) == 0)
    {
        sel = b0;
        if (PAD_REPEAT(1))
        {
            sel--;
            if (sel < 0)
                sel = 0;
        }
        if (PAD_REPEAT(2))
        {
            sel++;
            if ((u32)sel >= 2)
                sel = 1;
        }
        if (sel != b0)
        {
            b0 = sel;
            page = &D824[b0];
            sel = b2;
            if (sel >= page->count)
                sel = page->count - 1;
            if (fn_8_9FC(page->entries[sel].type) == 0)
            {
                do
                {
                    sel++;
                    if (sel >= page->count)
                        sel = 0;
                } while (fn_8_9FC(page->entries[sel].type) == 0);
            }
            b2 = sel;
        }
        sel = b2;
        page = &D824[b0];
        if (PAD_REPEAT(4))
        {
            do
            {
                sel++;
                if (sel >= page->count)
                    sel = 0;
            } while (fn_8_9FC(page->entries[sel].type) == 0);
        }
        if (PAD_REPEAT(8))
        {
            do
            {
                sel--;
                if (sel < 0)
                    sel = page->count - 1;
            } while (fn_8_9FC(page->entries[sel].type) == 0);
        }
        b2 = sel;
        entry = &D824[b0].entries[b2];
        switch (entry->type)
        {
        case 0:
            if (PAD[2].button & 0x100)
            {
                sub_mode_request = entry->subMode;
                return;
            }
            break;
        case 1:
            v4 = g_some_stage_id;
            if ((PAD[4].button & 0x200) || ((analog_inputs[0][0] & 0x200) && (PAD[0].button & 0x200)))
            {
                v4--;
                if (v4 < 1)
                    v4 = 0x1A4;
            }
            if ((PAD[4].button & 0x100) || ((analog_inputs[0][0] & 0x200) && (PAD[0].button & 0x100)))
            {
                v4++;
                if (v4 > 0x1A4)
                    v4 = 1;
            }
            if (v4 != g_some_stage_id)
                g_some_stage_id = v4;
            break;
        }
    }
}

void fn_8_770(void)
{
    struct MenuPage *page;
    int i;
    struct MenuEntry *entry;
    int j;
    int x;
    int y;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("TEST MODE");
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("\203e\203X\203g\203\202\201[\203h");
    for (page = D824, i = 0; i < 2U; i++, page++)
    {
        x = 0;
        y = 0;
        entry = page->entries;
        for (j = 0; j < page->count; j++, entry++)
        {
            int ex = entry->x;
            int ey = entry->y;

            if (ex <= 0)
                ex = x - ex;
            if (ey <= 0)
                ey = y - ey;
            x = ex;
            y = ey;
            debug_textdraw_set_pos(ex, ey);
            if (i == b0 && j == b2)
            {
                int c = (g_font_char_var_3 >> 3) % 6;

                if (c > 3)
                    c = 7 - c;
                c = ((c & 3) << 6) | ((c & 3) << 4) | ((c & 3) << 2) | 3;
                debug_textdraw_set_color(c);
                if (g_is_sjis_and_more_than_24_mib() != 0)
                {
                    fn_1_3E100(-3, 0);
                    debug_textdraw_vsprintf_buf_1("\201\237 ");
                }
                else
                {
                    fn_1_3E100(-2, 0);
                    debug_textdraw_put_buf_1("\034 ");
                }
                debug_textdraw_set_color(0xFF);
            }
            switch (entry->type)
            {
            case 2:
                debug_textdraw_set_color(0x33);
                debug_textdraw_put_buf_1(g_is_sjis_and_more_than_24_mib() != 0 ? entry->jp : entry->en);
                debug_textdraw_set_color(0xFF);
                break;
            case 0:
                debug_textdraw_put_buf_1(g_is_sjis_and_more_than_24_mib() != 0 ? entry->jp : entry->en);
                break;
            case 3:
                debug_textdraw_set_color(0x57);
                debug_textdraw_put_buf_1(g_is_sjis_and_more_than_24_mib() != 0 ? entry->jp : entry->en);
                debug_textdraw_set_color(0xFF);
                break;
            case 1:
                if (g_is_sjis_and_more_than_24_mib() != 0)
                {
                    debug_textdraw_put_buf_1(entry->jp);
                    fn_8_A18(g_some_stage_id);
                }
                else
                {
                    debug_textdraw_put_buf_1(entry->en);
                    debug_textdraw_vsprintf_buf_1("%d", g_some_stage_id);
                }
                break;
            }
        }
    }
}

int fn_8_9FC(int type)
{
    switch (type)
    {
    case 2:
        return 0;
    default:
        return 1;
    }
}

void fn_8_A18(int n)
{
    char *zero = "\202O";
    int v;
    int d;
    struct
    {
        u16 ch;
        s8 term;
    } buf;

    if (n < 0)
    {
        debug_textdraw_put_buf_1("\201|");
        n = -n;
    }
    v = n;
    d = 1;
    while (v > 0)
    {
        v /= 10;
        d *= 10;
    }
    buf.term = 0;
    v = n;
    d /= 10;
    while (d > 0)
    {
        buf.ch = *(u16 *)zero + v / d;
        debug_textdraw_put_buf_1((char *)&buf);
        v %= d;
        d /= 10;
    }
}

void fn_8_B1C(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        g_set_up_debugtext_buffer();
        b4 = 0;
        sub_mode_request = 0x77;
    }
}

void fn_8_B74(void)
{
    s32 v3;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        v3 = b4;
        if ((PAD_REPEAT(1)) && (v3 >= 0x10)) {
            v3 -= 0x10;
        }
        if ((PAD_REPEAT(2)) && (v3 < 0x10)) {
            v3 += 0x10;
        }
        if (PAD_REPEAT(8)) {
            switch (v3) {                       /* switch 1; irregular */
            case 0:                                 /* switch 1 */
                v3 = 0;
                break;
            case 16:                                /* switch 1 */
                v3 = 0x10;
                break;
            default:                                /* switch 1 */
                v3--;
                break;
            }
        }
        if (PAD_REPEAT(4)) {
            switch (v3) {                       /* switch 2; irregular */
            case 15:                                /* switch 2 */
                v3 = 0xF;
                break;
            case 31:                                /* switch 2 */
                v3 = 0x1F;
                break;
            default:                                /* switch 2 */
                v3++;
                break;
            }
        }
        b4 = v3;
        if (PAD[2].button & 0x100) {
            dip_switches ^= 1 << b4;
        }
    }
}

/* Unreferenced in the shipped REL; only its string literals survive. */
void dip_switch_draw(void)
{
    debug_textdraw_put_buf_1("\034");
    debug_textdraw_vsprintf_buf_1("%-18.18s  %s", b4, b4 ? " ON" : "OFF");
}

void fn_8_E40(void)
{
    b8 = 0;
    sub_mode_request = 0x75;
}

void fn_8_E64(void)
{
    s32 v30;
    void *v4;

    v30 = b8;
    if (PAD[0].button & 0x40) {
        if (PAD[2].button & 1) {
            v30--;
            if (v30 < 0) {
                v30 = 0;
            }
        }
        if (PAD[2].button & 2) {
            v30++;
            if (v30 >= 4) {
                v30 = 3;
            }
        }
    }
    b8 = v30;
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_vsprintf_buf_1("Input Test");
    debug_textdraw_set_pos(3, 3);
    debug_textdraw_set_color(0x33);
    debug_textdraw_vsprintf_buf_1("PAD           [%d]\n", v30);
    debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("\n");
    if (pad_status_groups[v30].unk0[0].button & 1) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_LEFT   %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 2) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_RIGHT  %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 4) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_DOWN   %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 8) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_UP     %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x10) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("TRIGGER_Z     %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x40) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("TRIGGER_L     %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x20) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("TRIGGER_R     %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x100) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_A      %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x200) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_B      %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x400) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_X      %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x800) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_Y      %3s\n", v4);
    if (pad_status_groups[v30].unk0[0].button & 0x1000) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("BUTTON_MENU   %3s\n", v4);
    debug_textdraw_vsprintf_buf_1("\n");
    debug_textdraw_vsprintf_buf_1("stickX       %4d\n", pad_status_groups[v30].unk0[0].stickX);
    debug_textdraw_vsprintf_buf_1("stickY       %4d\n", pad_status_groups[v30].unk0[0].stickY);
    debug_textdraw_vsprintf_buf_1("substickX    %4d\n", pad_status_groups[v30].unk0[0].substickX);
    debug_textdraw_vsprintf_buf_1("substickY    %4d\n", pad_status_groups[v30].unk0[0].substickY);
    debug_textdraw_vsprintf_buf_1("triggerLeft  %4d\n", pad_status_groups[v30].unk0[0].triggerLeft);
    debug_textdraw_vsprintf_buf_1("triggerRight %4d\n", pad_status_groups[v30].unk0[0].triggerRight);
    debug_textdraw_vsprintf_buf_1("analogA      %4d\n", pad_status_groups[v30].unk0[0].analogA);
    debug_textdraw_vsprintf_buf_1("analogB      %4d\n", pad_status_groups[v30].unk0[0].analogB);
    debug_textdraw_vsprintf_buf_1("err          %4d\n", pad_status_groups[v30].unk0[0].err);
    debug_textdraw_set_pos(0x17, 3);
    debug_textdraw_vsprintf_buf_1("APINPUT\n");
    debug_textdraw_vsprintf_buf_1("\n");
    if (*analog_inputs[v30] & 1) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("STICK_L       %3s\n", v4);
    if (*analog_inputs[v30] & 2) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("STICK_R       %3s\n", v4);
    if (*analog_inputs[v30] & 4) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("STICK_D       %3s\n", v4);
    if (*analog_inputs[v30] & 8) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("STICK_U       %3s\n", v4);
    if (*analog_inputs[v30] & 0x10) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("SUBSTICK_L    %3s\n", v4);
    if (*analog_inputs[v30] & 0x20) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("SUBSTICK_R    %3s\n", v4);
    if (*analog_inputs[v30] & 0x40) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("SUBSTICK_D    %3s\n", v4);
    if (*analog_inputs[v30] & 0x80) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("SUBSTICK_U    %3s\n", v4);
    if (*analog_inputs[v30] & 0x100) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("TRIGGER_L     %3s\n", v4);
    if (*analog_inputs[v30] & 0x200) {
        v4 = "ON";
    } else {
        v4 = "OFF";
    }
    debug_textdraw_vsprintf_buf_1("TRIGGER_R     %3s\n", v4);
    debug_textdraw_set_pos(0x16, 0x12);
    debug_textdraw_vsprintf_buf_1("PAD CHANGE:\n");
    debug_textdraw_vsprintf_buf_1("\n");
    debug_textdraw_vsprintf_buf_1("  TRIGGER_L\n");
    debug_textdraw_vsprintf_buf_1("      and\n");
    debug_textdraw_vsprintf_buf_1("    BUTTON_LEFT\n");
    debug_textdraw_vsprintf_buf_1("        or\n");
    debug_textdraw_vsprintf_buf_1("    BUTTON_RIGHT\n");
}

void fn_8_15FC(void)
{
    event_init(0x13);
    fn_1_36140();
    bC = 1;
    b10 = 0;
    b14 = (lbl_1_rodata_1284[b10].firstSoundId + 1);
    b18 = 0x7F;
    b19 = 0;
    b1A = 0;
    b1C = 0x2000;
    b1E = 0x2000;
    b20 = 0;
    b22 = 0;
    b23 = 0;
    b24 = 0;
    b25 = 0;
    b26 = 0;
    b28 = 0;
    b2A = 0x7F;
    b2B = 0;
    b2C = 0;
    b2E = 0;
    b30 = 0x64;
    b31 = 0;
    b34 = 0x78;
    b38 = 0;
    b39 = 0;
    sub_mode_request = 0x79;
}

static struct EffectParam DB8C[6] = {
    { "coloration", 0.05f, 0.0f, 1.0f },
    { "mix", 0.05f, 0.0f, 1.0f },
    { "time", 0.1f, 0.01f, 10.000000f },
    { "damping", 0.05f, 0.0f, 1.0f },
    { "preDelay", 0.005f, 0.0f, 0.1f },
    { "crosstalk", 0.05f, 0.0f, 1.0f },
};

static struct EffectParam DC0C[3] = {
    { "baseDelay", 1.0f, 5.0f, 15.0f },
    { "variation", 1.0f, 0.0f, 5.0f },
    { "period", 500.000000f, 0.0f, 10000.000000f },
};

static struct Effect DC4C[2] = {
    { DB8C, 6, 0, 0, 0, "REVERBH" },
    { DC0C, 3, 1, 0, 0, "CHORUS" },
};

void fn_8_171C(void)
{
    struct Effect *eff;
    struct EffectParam *param;
    float *fp;
    u32 *up;
    s8 v4;
    float rev[6];
    u32 cho[6];

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        if ((PAD[2].button & 4) || (PAD[4].button & 4)) {
                        if (++bC > 5) {
                bC = 0;
            }
        }
        if ((PAD[2].button & 8) || (PAD[4].button & 8)) {
                        if (--bC < 0) {
                bC = 5;
            }
        }
        switch (bC) {
        case 0:
            if (PAD_NAV(2)) {
                                if (++b10 >= 0x18) {
                    b10 = 0;
                }
            }
            if (PAD_NAV(1)) {
                                if (--b10 < 0) {
                    b10 = 0x17;
                }
            }
            if (PAD_NAV(2) | PAD_NAV(1)) {
                b14 = lbl_1_rodata_1284[b10].firstSoundId + 1;
            }
            if (PAD[2].button & 0x200) {
                g_something_calls_sndPopGroup();
            }
            if (PAD[2].button & 0x100) {
                SoundGroupLoad(b10);
                break;
            }
            break;
        case 1:
            if (PAD_NAV(2)) {
                b14++;
            }
            if (PAD_NAV(1)) {
                b14--;
            }
            if (lbl_1_rodata_148C[b14].flags & 0x2000) {
                if (b14 == lbl_1_rodata_1284[b10].firstSoundId) {
                    b14 = lbl_1_rodata_1284[b10 + 1].firstSoundId - 1;
                } else {
                    b14 = lbl_1_rodata_1284[b10].firstSoundId + 1;
                }
            }
            b19 = (s8) (1.05 * PAD[0].substickX);
            b1A = (s8) (1.05 * PAD[0].substickY);
            SoundPan(b14, b19, b1A);
            b1C = (8192.0 + (136.533 * PAD[0].stickX));
            SoundPitch(b14, b1C);
            b1E = (8192.0 + (136.533 * PAD[0].stickY));
            SoundDop(b14, b1E);
            b20 = (117.02 * PAD[0].triggerLeft);
            SoundMod(b14, b20);
            if (PAD[2].button & 0x400) {
                b24 = (b24 ^ 1);
            }
            if (b24 == 0) {
                b22 = (0.907 * PAD[0].triggerRight);
                SoundRev(b14, b22);
            } else {
                b23 = (0.907 * PAD[0].triggerRight);
                SoundCho(b14, b23);
            }
            g_player_id_for_sound = 0;
            if (PAD[2].button & 0x100) {
                SoundReqDirect(((b1A & 0x7F) << 0x19) | (((b19 & 0x7F) << 0x12) | (b14 | (((b18 - 0x7F) & 0x7F) << 0xB))));
            }
            if (PAD[2].button & 0x200) {
                SoundOff(b14);
            }
            if (PAD[2].button & 0x800) {
                event_sound_dest();
                break;
            }
            break;
        case 2:
            b26 = (s8)(int)(0.907 * PAD[0].triggerRight);
            b18 = (s8)(int)(127.0 - (0.907 * PAD[0].triggerLeft));
            b19 = (s8) (1.05 * PAD[0].substickX);
            b1A = (s8) (1.05 * PAD[0].substickY);
            SoundIcsReq(((b1A & 0x7F) << 0x19) | (b25 | ((b19 & 0x7F) << 0x12)), b26, b18);
            break;
        case 3:
            if (PAD_NAV(2)) {
                                if (++b28 >= 0x1B6) {
                    b28 = 0;
                }
            }
            if (PAD_NAV(1)) {
                                if (--b28 < 0) {
                    b28 = 0x1B5;
                }
            }
            b2A = (s8)(int)(127.0 - (0.907 * PAD[0].triggerLeft));
            fn_1_34B48(b28, b2A);
            b2B = (s8) (1.05 * PAD[0].substickX);
            b2C = (s8) (1.05 * PAD[0].substickY);
            fn_1_34C08(b28, b2B, b2C);
            if (PAD[2].button & 0x100) {
                call_SoftStreamSEReq_arg_0(b28, b2A, b2B, b2C);
            }
            if (PAD[2].button & 0x200) {
                g_stop_music_sound_debug(b28);
                break;
            }
            break;
        case 4:
            if (PAD_NAV(2)) {
                                if (++b2E >= 0x93) {
                    b2E = 0;
                }
            }
            if (PAD_NAV(1)) {
                                if (--b2E < 0) {
                    b2E = 0x92;
                }
            }
            if ((PAD[0].stickY > 0xF) && (b30 < 0x64)) {
                b30++;
            }
            if ((PAD[0].stickY < -0xF) && (b30 > 0)) {
                b30--;
            }
            if (PAD[2].button & 0x800) {
                g_change_music_volume(b2E, b34, b30);
            }
            v4 = fn_1_36534(b2E);
            if ((PAD[0].substickY > 0xF) && (v4 < 0x7F)) {
                v4++;
            }
            if ((PAD[0].substickY < -0xF) && (v4 > 0)) {
                v4--;
            }
            fn_1_365AC(b2E, v4);
            if ((PAD[0].stickX > 0xF) && (b34 < 0x258)) {
                b34 = (b34 + 1);
            }
            if ((PAD[0].stickX < -0xF) && (b34 > 0)) {
                b34 = (b34 - 1);
            }
            if (PAD[2].button & 0x400) {
                b31 = (b31 ^ 1);
            }
            if (PAD[2].button & 0x100) {
                if (b31 == 0) {
                    g_another_SoftStreamStart_wrapper_of_some_sort(b2E, b30);
                } else {
                    play_track_and_fade_out_other_tracks(b2E, b34, b30);
                }
            }
            if (PAD[2].button & 0x200) {
                g_something_with_stopping_music_or_sfx(b2E);
            }
            if (PAD[2].button & 0x10) {
                lbl_1_bss_24F64 ^= 1;
                if (lbl_1_bss_24F64 != 0) {
                    fn_1_36140();
                    break;
                }
            }
            break;
        case 5:
            if (PAD_NAV(2)) {
                                if (++b38 >= 2) {
                    b38 = 0;
                }
            }
            if (PAD_NAV(1)) {
                                if (--b38 < 0) {
                    b38 = 1;
                }
            }
            if (PAD_NAV(2) || PAD_NAV(1)) {
                b39 = 0;
            }
            if (PAD[2].button & 0x400) {
                switch (b38) {
                case 0:
                    g_snd_dbg_call_sndAuxCallbackUpdateSettingsReverbHI_2();
                    break;
                case 1:
                    g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_1();
                    break;
                }
            }
            eff = &DC4C[b38];
            fn_8_336C(rev, cho);
            fp = &rev[b39];
            up = &cho[b39];
            param = &eff->params[b39];
            if (analog_inputs[0][4] & 0x40) {
                                if (++b39 >= (s8)eff->count) {
                    b39 = 0;
                }
            }
            if (analog_inputs[0][4] & 0x80) {
                                if (--b39 < 0) {
                    b39 = eff->count - 1;
                }
            }
            if (analog_inputs[0][4] & 0x20) {
                if ((s8)eff->id == 0) {
                    *fp += param->step;
                } else {
                    *up = (u32) (*up + param->step);
                }
            }
            if (analog_inputs[0][4] & 0x10) {
                if ((s8)eff->id == 0) {
                    *fp -= param->step;
                } else {
                    *up = (u32) (*up - param->step);
                }
            }
            if ((analog_inputs[0][4] & 0x20) || (analog_inputs[0][4] & 0x10)) {
                if ((s8)eff->id == 0) {
                    *fp = *fp < param->min ? param->min : (*fp > param->max ? param->max : *fp);
                } else {
                    *up = *up < param->min ? param->min : (*up > param->max ? param->max : *up);
                }
                switch (b38) {
                case 0:
                    g_snd_dbg_call_sndAuxCallbackupdateSettingsReverbHI_1((struct ReverbHI *)rev);
                    break;
                case 1:
                    g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_2((struct Chorus *)cho);
                    break;
                }
            }
            break;
        }
    }
}

void sound_test_text_begin(void)
{
    textdraw_reset();
    textdraw_set_font(2);
    textdraw_set_pos_for_aram_font_test(3.0f, 29.0f);
    textdraw_set_font(3);
    textdraw_set_flags(0x80000000);
    textdraw_set_drop_shadow();
    textdraw_set_spacing(0.0f, 3.0f);
    textdraw_set_scale(0.75f, 0.75f);
}

void fn_8_2AB4(void)
{
    int i;
    u8 v5;
    void *v4;
    struct Effect *eff;
    const struct SoundEffectDesc *desc;
    float rev[6];
    u32 cho[6];

    desc = &lbl_1_rodata_148C[b14];
    textdraw_reset();
    textdraw_set_font(2);
    textdraw_set_pos_for_aram_font_test(3.0f, 1.0f);
    textdraw_print("SOUND TEST\n\n");
    textdraw_set_mul_color(0x804080);
    textdraw_print("GROUP [ ");
    for (i = 0; i < 0x10; i++) {
        textdraw_printf("%d ", (&lbl_1_bss_16040)[i]);
    }
    textdraw_print("]\n");
    textdraw_print("STACK [ ");
    for (i = 0; i < 8; i++) {
        textdraw_printf("%2d ", (&lbl_1_bss_16084)[i]);
    }
    textdraw_print("]\n\n");
    textdraw_printf(" RAM use:%08X\n", g_debug_sound_ram_usage);
    textdraw_printf("ARAM use:%08X / %08X\n", g_debug_sound_aram_usage, 0x6C0000);
    textdraw_set_pos_for_aram_font_test(3.0f, 9.0f);
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_printf("GRP :%2d %s\n", b10, lbl_1_rodata_1284[b10].name);
    textdraw_printf("FX  :%4d(%4d/%4d) %s\n", b14, lbl_1_rodata_1284[b10].firstSoundId + 1, lbl_1_rodata_1284[b10 + 1].firstSoundId - 1, desc->name);
    textdraw_printf("\n");
    textdraw_printf("ICS :\n", b25);
    textdraw_printf("STRM SE  :%2d %s\n", b28, fn_1_34AB0(b28));
    textdraw_printf("STRM BGM :%2d %s\n", b2E, fn_1_36488(b2E));
    textdraw_printf("EFC : %d %s\n", b38, DC4C[b38].name);
    textdraw_print("\n\n");
    textdraw_set_mul_color(0x408040);
    switch (bC) {
    case 0:
        sound_test_text_begin();
        textdraw_print("\203\265:\203\201\203j\203\205\201[/\203O\203\213\201[\203vID\203Z\203\214\203N\203g\n");
        textdraw_print("\203\237:\203O\203\213\201[\203v LOAD\201@\203\240:\203X\203^\203b\203N FREE\n");
        break;
    case 1:
        textdraw_printf("pan:%3d %3d\n", b19, b1A);
        textdraw_printf("pit:%04X\n", b1C);
        textdraw_printf("dop:%04X\n", b1E);
        textdraw_printf("mod:%04X\n", b20);
        if (b24 == 0) {
            v5 = b22;
        } else {
            v5 = b23;
        }
        if (b24 == 0) {
            v4 = "rev";
        } else {
            v4 = "cho";
        }
        textdraw_printf("%s:%3d\n", v4, v5);
        textdraw_print("\n");
        sound_test_text_begin();
        textdraw_print("\203\265:\203\201\203j\203\205\201[/SEID\203Z\203\214\203N\203g\n");
        textdraw_print("\203\237:SE\203\212\203N\203G\203X\203g\201@\203\240:SE OFF\201@\203\262:ALL SE OFF\n");
        textdraw_print("\203\246:pan\201@\203\255:pit/dop\n");
        textdraw_print("\203\257:rev/cho(\203G\203t\203F\203N\203g)\201@\203\264:\203G\203t\203F\203N\203g\202\253\202\350\202\251\202\246\201@\203\241:mod\n");
        break;
    case 2:
        textdraw_printf("ctr:%3d\n", b26);
        textdraw_printf("vol:%3d\n", b18);
        textdraw_printf("pan:%3d %3d\n", b19, b1A);
        sound_test_text_begin();
        textdraw_print("\203\265:\203\201\203j\203\205\201[\203Z\203\214\203N\203g\n");
        textdraw_print("\203\257:ICS \203R\203\223\203g\203\215\201[\203\213\n");
        textdraw_print("\203\241:vol\n");
        textdraw_print("\203\246:pan\n");
        break;
    case 3:
        textdraw_printf("vol:%3d\n", b2A);
        textdraw_printf("pan:%3d %3d\n", b2B, b2C);
        textdraw_print("\n");
        for (i = 0; i < 0x93; i++) {
            if (fn_1_34A28(i) != -1) {
                textdraw_printf("port:%d %2d:%-12s ", fn_1_34A28(i), i, fn_1_34AFC(i));
                textdraw_print("\n");
            }
        }
        sound_test_text_begin();
        textdraw_print("\n\n");
        textdraw_print("\203\265:\203\201\203j\203\205\201[/SE\203Z\203\214\203N\203g\n");
        textdraw_print("\203\237:SE\203\212\203N\203G\203X\203g\201@\203\240:SE OFF\n");
        textdraw_print("\203\241:vol\n");
        textdraw_print("\203\246:pan\n");
        break;
    case 4:
        if (lbl_1_bss_24F64 != 0) {
            textdraw_print("BGM OFF\n");
        } else {
            if (b31 != 0) {
                v4 = "AutoCrossFade";
            } else {
                v4 = "Normal";
            }
            textdraw_printf("req mode :%s\n", v4);
            textdraw_printf("vol      :%3d%\n", b30);
            textdraw_printf("fade time:%d\n", b34);
            textdraw_print("\n");
            textdraw_print("port id:name         beat vol[base per]\n");
            for (i = 0; i < 0x93; i++) {
                if (g_maybe_related_to_music_crossfading(i) != -1) {
                    textdraw_printf("   %d %2d:%-12s ", g_maybe_related_to_music_crossfading(i), i, fn_1_364E8(i));
                    textdraw_printf("%04d [%d/127 %d/100] ", fn_1_363C8(i), fn_1_36534(i), fn_1_36550(i));
                    textdraw_print("\n");
                }
            }
        }
        sound_test_text_begin();
        textdraw_print("\203\265:\203\201\203j\203\205\201[/BGM\203Z\203\214\203N\203g\n");
        textdraw_print("\203\237:BGM\203\212\203N\203G\203X\203g\201@\203\264:\203\212\203N\203G\203X\203gMODE\201@\203\240:BGM OFF\n");
        textdraw_print("\203\262:vol\203Z\203b\203g(\203t\203F\201[\203h\203C\203\223/\203A\203E\203g)\n");
        textdraw_print("\203\255:vol/fade time\n");
        textdraw_print("\203\246:base vol\n");
        textdraw_print("\203\244:BGM ON/OFF\n");
        break;
    case 5:
        eff = &DC4C[b38];
        fn_8_336C(rev, cho);
        for (i = 0; i < (s8)eff->count; i++) {
            if (b39 == i) {
                textdraw_print("\034 ");
            } else {
                textdraw_print("  ");
            }
            if ((s8)eff->id == 0) {
                textdraw_printf("%-10s %f\n", eff->params[i].name, rev[i]);
            } else {
                textdraw_printf("%-10s %d\n", eff->params[i].name, cho[i]);
            }
        }
        sound_test_text_begin();
        textdraw_print("\203\265:\203\201\203j\203\205\201[/\203G\203t\203F\203N\203^\201[\203Z\203\214\203N\203g\n");
        textdraw_print("\203\246:\203G\203t\203F\203N\203g\203\201\203j\203\205\201[/\203p\203\211\203\201\201[\203^\n");
        textdraw_print("\203\264:\203p\203\211\203\201\201[\203^\203\212\203Z\203b\203g\n");
        break;
    }
    textdraw_set_font(2);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_set_pos_for_aram_font_test(1.0f, bC + 9 + (bC >= 2 ? 1 : 0));
    textdraw_set_mul_color(0x804040);
    textdraw_print("\034");
}

void fn_8_336C(void *rev, void *cho)
{
    struct ReverbHI r;
    struct Chorus c;

    switch (b38)
    {
    case 0:
        r = fn_1_36E18();
        memcpy_handler(rev, &r, sizeof(r));
        break;
    case 1:
        c = fn_1_36E7C();
        memcpy_handler(cho, &c, sizeof(c));
        break;
    }
}

void fn_8_33FC(void)
{
    event_init(0x16);
    fn_8_16EEC();
    sub_mode_request = 0x9A;
}

void fn_8_3434(void)
{
    fn_8_16F18();
}

static struct TexFormat D111C[7] = {
    { 6, "GX_TF_RGBA8" },
    { 0, "GX_TF_I4" },
    { 1, "GX_TF_I8" },
    { 2, "GX_TF_IA4" },
    { 3, "GX_TF_IA8" },
    { 4, "GX_TF_RGB565" },
    { 5, "GX_TF_RGB5A3" },
};

void fn_8_3454(void)
{
    s32 tr4;

    s_copy.fmt = 0;
    s_copy.alpha = 0xC0;
    tr4 = GXGetTexBufferSize(0x280, 0x1E0, 6, 0, 0);
    if (s_copy.buf == NULL) {
        s_copy.buf = alloc_from_heap_or_panic(currentHeap, tr4, "test_mode.c", 0x609);
    }
    if (s_copy.buf == NULL) {
        sub_mode_request = 0x72;
        return;
    }
    GXInitTexObj(&s_copy.tex, s_copy.buf, current_render_mode->fbWidth, current_render_mode->efbHeight, D111C[s_copy.fmt].format, 0, 0, 0);
    fn_8_CEB4();
    camera_set_state(2);
    sub_mode_request = 0x7B;
}

void fn_8_3594(void)
{
    if (PAD[2].button & 1) {
        s_copy.fmt = (s_copy.fmt - 1);
        if (s_copy.fmt < 0) {
            s_copy.fmt = 6;
        }
    }
    if (PAD[2].button & 2) {
        s_copy.fmt = (s_copy.fmt + 1);
        if ((u32) s_copy.fmt > 6) {
            s_copy.fmt = 0;
        }
    }
    if ((PAD[2].button & 8) && (s_copy.alpha < 0xFF)) {
        s_copy.alpha++;
    }
    if ((PAD[2].button & 4) && (s_copy.alpha > 0)) {
        s_copy.alpha--;
    }
    fn_8_CFDC();
}

void fn_8_36E4(void)
{
    if (s_copy.buf != NULL) {
        OSFreeToHeap(currentHeap, s_copy.buf);
        s_copy.buf = NULL;
    }
}

void fn_8_3740(void)
{
    Mtx44 mtx;
    GXColor c;
    int tev;
    int texGen;

    C_MTXPerspective(mtx, 59.996338f, 1.3333334f, 0.1f, 20000.0f);
    g_something_with_GXSetProjectionv(mtx, 0);
    c.r = 0xFF;
    c.g = 0xFF;
    c.b = 0xFF;
    c.a = s_copy.alpha;
    GXSetChanMatColor(4, c);
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.a = 0;
    GXSetChanAmbColor(4, c);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 2, 1);
    GXSetNumChans_cached(1);
    GXInvalidateTexAll();
    GXLoadTexObj_cached(&s_copy.tex, 0);
    tev = 0;
    texGen = 0;
    GXSetTexCoordGen2_cached(texGen, 0, 0, 0x1E, 0, 0x7D);
    mtxa_from_identity();
    mtxa->mtxA[0][0] = -0.6495191f;
    mtxa->mtxA[0][2] = 0.5f;
    mtxa->mtxA[1][1] = 0.86602545f;
    mtxa->mtxA[1][2] = 0.5f - (0.5f / current_render_mode->efbHeight);
    GXLoadTexMtxImm(mtxa->mtxA, 0x1E, 0);
    g_GXSetTevOrder_wrapper(tev, texGen, 0, 4);
    GXSetTevColorIn_cached(tev, 0xF, 0xA, 8, 0xF);
    GXSetTevColorOp_cached(tev, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(tev, 7, 7, 7, 5);
    GXSetTevAlphaOp_cached(tev, 0, 0, 0, 1, 0);
    g_GXSetTevIndirect_zero_if_different(tev);
    tev++;
    texGen++;
    GXSetNumTevStages_cached(tev);
    GXSetNumTexGens_cached(texGen);
    GXSetNumIndStages_cached(0);
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 7, 0);
    fn_1_E2F1C();
    g_something_with_GX_vtx_desc(0x200);
    GXSetVtxAttrFmt(1, 9, 1, 4, 0);
    mtxa_from_identity();
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXBegin(0x80, 1, 4);
    GXPosition3f32(-0.8467804f, 0.6350853f, -1.0f);
    GXPosition3f32(0.8467804f, 0.6350853f, -1.0f);
    GXPosition3f32(0.8467804f, -0.6350853f, -1.0f);
    GXPosition3f32(-0.8467804f, -0.6350853f, -1.0f);
    GXSetZMode_cached(1, 1, 1);
    camera_apply_main_projection();
}

void blur_test_draw(void)
{
    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("BLUR TEST\n");
    debug_textdraw_vsprintf_buf_1("FORMAT : %s\n", D111C[s_copy.fmt].name);
    debug_textdraw_vsprintf_buf_1("ALPHA  : %d\n", s_copy.alpha);
    game_main_draw_func();
    fn_8_3740();
    GXSetTexCopySrc_cached(0, 0, current_render_mode->fbWidth, current_render_mode->efbHeight);
    GXSetTexCopyDst_cached(current_render_mode->fbWidth, current_render_mode->efbHeight, D111C[s_copy.fmt].format, 0);
    GXCopyTex(s_copy.buf, 0);
    GXInitTexObj(&s_copy.tex, s_copy.buf, current_render_mode->fbWidth, current_render_mode->efbHeight, D111C[s_copy.fmt].format, 0, 0, 0);
    GXInitTexObjLOD(&s_copy.tex, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f);
    return;
}

void fn_8_3AD0(void)
{
    fn_8_FDF0();
    dest_all_events();
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(2);
    sub_mode_request = 0x7D;
}

void fn_8_3B18(void)
{
    fn_8_10070();
}

void shadow_test_draw(void)
{
    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("SHADOW TEST\n");
    fn_8_100FC();
    fn_8_10464();
    handle_test_camera();
    return;
}

void fn_8_3B38(void)
{
    fn_8_1226C();
    camera_set_state(2);
    sub_mode_request = 0x7F;
}

void fn_8_3B70(void)
{
    fn_8_12404();
}

void fn_8_3B90(void)
{
    fn_8_12F1C();
    dest_all_events();
    fn_1_56C18();
    load_stage(g_some_stage_id);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(4);
    event_init(9);
    event_init(5);
    event_init(7);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    enable_cameras_for_active_players();
    camera_set_state(2);
    sub_mode_request = 0x81;
}

void fn_8_3C4C(void)
{
    fn_8_13080();
}

static struct LoadSet D1230[4] = {
    { "common", "init/common_p.lz", "init/common.lz" },
    { "face", "init/face_p.lz", "init/face.lz" },
    { "sel", "init/sel_p.lz", "init/sel.lz" },
    { "minitest", "init/minit_p.lz", "init/minit.lz" },
};

void fn_8_3C6C(void)
{
    struct LoadSet *tr6;

    s_nl.model = 0;
    s_nl.set = 0;
    if (s_nl.tpl != NULL) {
        free_nl2ngc_tpl_buf_to_heap(s_nl.tpl);
        s_nl.tpl = NULL;
    }
    if (s_nl.obj != NULL) {
        OSFreeToHeap(currentHeap, s_nl.obj);
        s_nl.obj = NULL;
    }
    tr6 = &D1230[s_nl.set];
    load_nlobj(&s_nl.obj, &s_nl.tpl, tr6->tpl, tr6->gma);
    dest_all_events();
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(2);
    sub_mode_request = 0x83;
}

void fn_8_3D84(void)
{
    free_nlobj(&s_nl.obj, &s_nl.tpl);
}

void fn_8_3DBC(void)
{
    struct DiscFileInfo info;
    u32 size;
    struct LoadSet *tr30;
    int v4;

    v4 = s_nl.set;
    if (PAD[2].button & 8) {
        v4++;
        if ((u32)v4 > 3) {
            v4 = 0;
        }
    }
    if (PAD[2].button & 4) {
        v4--;
        if (v4 < 0) {
            v4 = 3;
        }
    }
    if (v4 != s_nl.set) {
        s_nl.model = 0;
        s_nl.set = v4;
        free_nlobj(&s_nl.obj, &s_nl.tpl);
        if (s_nl.tpl != NULL) {
            free_nl2ngc_tpl_buf_to_heap(s_nl.tpl);
            s_nl.tpl = NULL;
        }
        if (s_nl.obj != NULL) {
            OSFreeToHeap(currentHeap, s_nl.obj);
            s_nl.obj = NULL;
        }
        tr30 = &D1230[s_nl.set];
        if (DVDOpen(tr30->tpl, &info) == 0) {
            sub_mode_request = 0x72;
            return;
        }
        size = (info.length + 0x1F) & 0xFFFFFFE0;
        DVDClose(&info);
        s_nl.obj = OSAllocFromHeap(currentHeap, size);
        if (s_nl.obj == NULL) {
            sub_mode_request = 0x72;
            return;
        }
        load_nlobj(&s_nl.obj, &s_nl.tpl, tr30->tpl, tr30->gma);
    }
    if (PAD[2].button & 1) {
        s_nl.model = (s_nl.model - 1);
        if (s_nl.model < 0) {
            s_nl.model = 0;
            while (s_nl.obj->models[s_nl.model] != 0) {
                s_nl.model = (s_nl.model + 1);
            }
            s_nl.model = (s_nl.model - 1);
        }
    }
    if (PAD[2].button & 2) {
        s_nl.model = (s_nl.model + 1);
        if (s_nl.obj->models[s_nl.model] == 0) {
            s_nl.model = 0;
        }
    }
}

void bitmap_test_unused(float *f)
{
    static const GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};

    GXSetChanMatColor(4, white);
    f[0] = 360.0f;
    f[1] = 10.0f;
    f[2] = -8.0f;
    f[3] = 2.5f;
    f[4] = 1.25f;
    f[5] = -18.0f;
    f[6] = 10.25f;
}
/* Unreferenced in the shipped REL; only its string literals survive. */
void test_model_draw(void)
{
    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("Category      :%s\n", D1230[s_nl.set].dir);
    if (s_nl.obj == 0) {
        debug_textdraw_vsprintf_buf_1("no data\n");
        return;
    }
    debug_textdraw_vsprintf_buf_1("name          :%s\n", ((char **)s_nl.obj->models[s_nl.model])[-2]);
    debug_textdraw_vsprintf_buf_1("size          :%d\n", *((int **)s_nl.obj->models[s_nl.model])[-1]);
    mtxa_from_mtxb();
    nl2ngc_draw_model_sort_translucent(s_nl.obj->models[s_nl.model]);
    handle_test_camera();
}

void fn_8_40F8(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        s_bmp.category = 0;
        s_bmp.id = 0;
        s_bmp.row = 1;
        s_bmp.baseR = 0xFF;
        s_bmp.baseG = 0xFF;
        s_bmp.baseB = 0xFF;
        s_bmp.ofsR = 0;
        s_bmp.ofsG = 0;
        s_bmp.ofsB = 0;
        s_bmp.zang = 0;
        s_bmp.trnsl = 1.0f;
        s_bmp.zoomX = 1.0f;
        s_bmp.zoomY = 1.0f;
        s_bmp.step = 0.125f;
        s_bmp.u0 = s_bmp.v0 = 0.0f;
        s_bmp.u1 = s_bmp.v1 = 1.0f;
        bAC = 0;
        sub_mode_request = 0x85;
    }
}

static struct BmpField D1414[17] = {
    { 1, "category   : %s\n", &s_bmp.category },
    { 2, "ID         : %3d\n      name : bmp_%s\n      size : %3d x %3d\n\n", &s_bmp.id },
    { 4, "base red   : %3d\n", &s_bmp.baseR },
    { 4, "base green : %3d\n", &s_bmp.baseG },
    { 4, "base blue  : %3d\n", &s_bmp.baseB },
    { 4, "ofs  red   : %3d\n", &s_bmp.ofsR },
    { 4, "ofs  green : %3d\n", &s_bmp.ofsG },
    { 4, "ofs  blue  : %3d\n", &s_bmp.ofsB },
    { 5, "base zang  : %04hX\n", &s_bmp.zang },
    { 6, "trnsl      : %4.2f\n", &s_bmp.trnsl },
    { 7, "zm_x       : %4.1f\n", &s_bmp.zoomX },
    { 7, "zm_y       : %4.1f\n", &s_bmp.zoomY },
    { 8, "texture u0 : %4.3f\n", &s_bmp.u0 },
    { 8, "texture v0 : %4.3f\n", &s_bmp.v0 },
    { 8, "texture u1 : %4.3f\n", &s_bmp.u1 },
    { 8, "texture v1 : %4.3f\n", &s_bmp.v1 },
    { 0, NULL, NULL },
};

void fn_8_4254(void)
{
float vf1;
int v3;
struct BmpField *tr31;
s32 tr0;
s32 v30;
struct BitmapGroup *tr3;
s32 v4;
s32 v6;
int v0;

if ((u32)(g_some_other_flags & 0xA) == 0) {
    if (PAD[2].button & 0x100) {
        bAC ^= 1;
    }
    if (PAD_REPEAT(8)) {
        if (--s_bmp.row < 0) {
            for (v6 = 0; D1414[v6].type != 0; v6++) {
            }
            s_bmp.row = (v6 - 1);
        }
    }
    if (PAD_REPEAT(4)) {
        s_bmp.row++;
            if (D1414[s_bmp.row].type == 0) {
            s_bmp.row = 0;
        }
    }
    tr31 = &D1414[s_bmp.row];
    switch (tr31->type) {
        case 1:
            v30 = *(int *)tr31->value;
            if (PAD_REPEAT(1)) {
                v30--;
                if (v30 < 0) {
                    v30 = 0x18;
                }
            }
            if (PAD_REPEAT(2)) {
                v30++;
                if (v30 >= 0x19) {
                    v30 = 0;
                }
            }
            if (v30 != s_bmp.category) {
                if (s_bmp.category != 0) {
                    call_free_bmp_by_id(s_bmp.category);
                }
                load_bmp_by_id(v30);
                *(int *)tr31->value = v30;
                s_bmp.id = 0;
                return;
            }
            break;
        case 2:
            v4 = *(int *)tr31->value;
            tr3 = &bmp_infos[s_bmp.category];
            if (PAD_REPEAT(1)) {
                v4--;
                if (v4 < 0) {
                    v4 = (s16) (tr3->tpl->numTextures - 1);
                }
            }
            if (PAD_REPEAT(2)) {
                v4++;
                if ((u32) v4 > (u32) (tr3->tpl->numTextures - 1)) {
                    v4 = 0;
                }
            }
            *(int *)tr31->value = v4;
            return;
        case 4:
            if (PAD[0].button & 0x20) {
                v3 = 0xA;
            } else {
                v3 = 1;
            }
            v0 = *(u8 *)tr31->value;
            if (PAD_REPEAT(2)) {
                v0 += v3;
            }
            if (PAD_REPEAT(1)) {
                v0 -= v3;
            }
            v0 = v0 < 0 ? 0 : (v0 > 0xFF ? 0xFF : v0);
            *(u8 *)tr31->value = v0;
            return;
        case 5:
            v3 = *(s16 *)tr31->value;
            if (PAD_REPEAT(2)) {
                v3 += 0x80;
            }
            if (PAD_REPEAT(1)) {
                v3 -= 0x80;
            }
            *(s16 *)tr31->value = (s16)v3;
            return;
        case 6:
            vf1 = *(float *)tr31->value;
            if (PAD_REPEAT(2)) {
                vf1 += 0.01;
            }
            if (PAD_REPEAT(1)) {
                vf1 -= 0.01;
            }
            vf1 = vf1 < 0.0 ? 0.0 : (vf1 > 1.0 ? 1.0 : vf1);
            *(float *)tr31->value = vf1;
            return;
        case 7:
            vf1 = *(float *)tr31->value;
            if (PAD_REPEAT(2)) {
                vf1 += 0.1;
            }
            if (PAD_REPEAT(1)) {
                vf1 -= 0.1;
            }
            *(float *)tr31->value = vf1;
            return;
        case 8:
            vf1 = *(float *)tr31->value;
            if (PAD_REPEAT(2)) {
                vf1 += s_bmp.step;
            }
            if (PAD_REPEAT(1)) {
                vf1 -= s_bmp.step;
            }
            *(float *)tr31->value = vf1;
            break;
    }
}
}

static int s_unused = -1;

void fn_8_4D5C(void)
{
    struct SpriteDrawRequest req;
    struct BmpField *v29;
    int v28;
    struct BitmapGroup *tr27;
    struct TPLTextureHeader *tr26;
    int v;
    int w;
    int h;

    tr27 = &bmp_infos[s_bmp.category];
    tr26 = &tr27->tpl->texHeaders[s_bmp.id];
    if ((PAD[0].button & 0x200) == 0) {
        debug_textdraw_set_pos(3, 5);
        v28 = 0;
        v29 = D1414;
        while (v29->type != 0) {
            if (v28 == s_bmp.row) {
                fn_1_3E100(-2, 0);
                debug_textdraw_set_color(0xC3);
                debug_textdraw_put_buf_1("\034 ");
                debug_textdraw_set_color(0xFF);
            }
            switch (v29->type) {
            case 1:
                debug_textdraw_vsprintf_buf_1(v29->fmt, tr27->name);
                break;
            case 2:
                v = *(int *)v29->value;
                debug_textdraw_vsprintf_buf_1(v29->fmt, v, bmp_tex_names[s_bmp.category][v], tr26->width, tr26->height);
                break;
            case 4:
                debug_textdraw_vsprintf_buf_1(v29->fmt, *(u8 *)v29->value);
                break;
            case 5:
                debug_textdraw_vsprintf_buf_1(v29->fmt, *(s16 *)v29->value);
                break;
            case 6:
            case 7:
            case 8:
                debug_textdraw_vsprintf_buf_1(v29->fmt, *(float *)v29->value);
                break;
            }
            v28++;
            v29++;
        }
    }
    if (tr26->width != 0 && tr26->height != 0) {
        req.bmpId = s_bmp.id | (s_bmp.category << 8);
        req.x = 320.0f;
        req.y = 240.0f;
        req.z = 0.1f;
        req.scaleX = s_bmp.zoomX * (s_bmp.u1 - s_bmp.u0);
        req.scaleY = s_bmp.zoomY * (s_bmp.v1 - s_bmp.v0);
        req.u1 = s_bmp.u0;
        req.v1 = s_bmp.v0;
        req.u2 = s_bmp.u1;
        req.v2 = s_bmp.v1;
        req.rotation = s_bmp.zang;
        req.opacity = s_bmp.trnsl;
        req.unk30 = -1;
        req.flags = 0x2100A;
        req.mulColor = ((u8)(255.0f * s_bmp.trnsl) << 24) | (s_bmp.baseR << 16) | (s_bmp.baseG << 8) | s_bmp.baseB;
        req.addColor = (s_bmp.ofsR << 16) | (s_bmp.ofsG << 8) | s_bmp.ofsB;
        set_ui_widescreen_scale_mtx(0x140);
        draw_sprite_draw_request(&req);
        reset_ui_widescreen_scale_mtx();
    }
    if ((PAD[0].button & 0x200) == 0) {
        w = tr26->width;
        h = tr26->height;
        textdraw_reset();
        textdraw_set_pos(req.x - w / 2 - 8.0f, req.y - h / 2 - 8.0f);
        textdraw_print("+");
        textdraw_set_pos(req.x + w / 2, req.y - h / 2 - 8.0f);
        textdraw_print("+");
        textdraw_set_pos(req.x - w / 2 - 8.0f, req.y + h / 2);
        textdraw_print("+");
        textdraw_set_pos(req.x + w / 2, req.y + h / 2);
        textdraw_print("+");
    }
}

void fn_8_52F4(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        bB0 = 0;
        bB4 = 0;
        sub_mode_request = 0x87;
    }
}

static struct FontSample D154C[7] = {
    { 0, "HELLO, WORLD" },
    { 1, "HELLO, WORLD" },
    { 2, "HELLO, WORLD\n\030\026\026\026\031\n\027   \027\n\032\026\026\026\033" },
    { 4, "HELLO, WORLD\n\030\026\026\026\031\n\027   \027\n\032\026\026\026\033" },
    { 3, "\203n\203\215\201[\203\217\201[\203\213\203h" },
    { 9, "HELLO, WORLD" },
    { -1, NULL },
};

static struct FontSample D168C[4] = {
    { 142, "h/OOMOJI a/ABCDEFG HIJKLMN\nOPQRSTU VWXYZ\nk/KOMOJI a/abcdefg hijklmn\nopqrstu vwxyz\n0123456789" },
    { 143, "h/OOMOJI a/ABCDEFG HIJKLMN\nOPQRSTU VWXYZ\nk/KOMOJI a/abcdefg hijklmn\nopqrstu vwxyz\n0123456789\np/BOTANN_A/ANNDO/LE/GENZAI/JIBUNN/KOME/BUTTON_+/" },
    { 144, "p/BOTANN_A/BUTTON_+/" },
    { -1, NULL },
};

static struct FontSample D16B4[3] = {
    { 21, "\001\002\020\010\005" },
    { 22, "\001\002\020\010\005" },
    { -1, NULL },
};

static struct FontSample D16CC[1] = {
    { -1, NULL },
};

static struct FontSample D16E4[7] = {
    { 52, "0123" },
    { 53, "0123456" },
    { 54, "0123456" },
    { 55, "0123456" },
    { 56, "0123456" },
    { 57, "0123456" },
    { -1, NULL },
};

static struct FontSample D1734[6] = {
    { 93, "0123456789" },
    { 94, "0123456789" },
    { 95, "0123456789:" },
    { 96, "0123456789:" },
    { 97, "0123456789:" },
    { -1, NULL },
};

static struct FontGroup D17A8[7] = {
    { D154C, "COMMON", 0 },
    { D168C, "COMMON(JIS)", 0 },
    { D16B4, "COMMON(ICON)", 0 },
    { D16CC, "RANKING", 3 },
    { D16E4, "SELECTER", 23 },
    { D1734, "MAIN GAME", 5 },
    { NULL, NULL, 0 },
};

void fn_8_5340(void)
{
    struct FontSample *tr3;
    s32 v31;
    s32 v4;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        v31 = bB4;
        if (PAD_REPEAT(2)) {
            if (D17A8[v31].unk8 != 0) {
                call_free_bmp_by_id(D17A8[v31].unk8);
            }
            v31++;
            if (D17A8[v31].samples == NULL) {
                v31 = 0;
            }
            if (D17A8[v31].unk8 != 0) {
                load_bmp_by_id(D17A8[v31].unk8);
            }
        }
        if (PAD_REPEAT(1)) {
            if (D17A8[v31].unk8 != 0) {
                call_free_bmp_by_id(D17A8[v31].unk8);
            }
            v31--;
            if (v31 < 0) {
                for (v31 = 0; D17A8[v31].samples != NULL; v31++) {
                }
                v31 = v31 - 1;
            }
            if (D17A8[v31].unk8 != 0) {
                load_bmp_by_id(D17A8[v31].unk8);
            }
        }
        if (v31 != bB4) {
            bB0 = 0;
            bB4 = v31;
        }
        tr3 = D17A8[bB4].samples;
        v4 = bB0;
        if (PAD_REPEAT(4)) {
            v4++;
            if (tr3[v4].font == -1) {
                v4 = 0;
            }
        }
        if (PAD_REPEAT(8)) {
            v4--;
            if (v4 < 0) {
                for (v4 = 0; tr3[v4].font != -1; v4++) {
                }
                v4 = v4 - 1;
            }
        }
        bB0 = v4;
    }
}

void fn_8_5740(void)
{
    struct SpriteDrawRequest req;
    struct FontSample *v29;
    int v28;
    struct FontParams *tr27;

    tr27 = &FONT_DEFINITIONS[D17A8[bB4].samples[bB0].font];
    debug_textdraw_set_pos(2, 2);
    debug_textdraw_put_buf_1(D17A8[bB4].name);
    textdraw_reset();
    v28 = 0;
    v29 = D17A8[bB4].samples;
    while (v29->font >= 0) {
        if (v28 == bB0) {
            debug_textdraw_set_color(0x33);
            debug_textdraw_set_pos(1, v28 + 4);
            debug_textdraw_vsprintf_buf_1("%18s *", FONT_NAME_LIST[v29->font]);
            debug_textdraw_set_color(0xFF);
        } else {
            debug_textdraw_set_pos(1, v28 + 4);
            debug_textdraw_vsprintf_buf_1("%18s", FONT_NAME_LIST[v29->font]);
        }
        v29++;
        v28++;
    }
    v29 = &D17A8[bB4].samples[bB0];
    textdraw_set_font(v29->font);
    textdraw_set_pos(lbl_1_bss_6FB6C + 0x10, lbl_1_bss_6FB68 + 0x12C);
    textdraw_print(v29->text);
    if (tr27->bmpId >= 0 && (PAD[0].button & 0x200) == 0) {
        req.bmpId = g_get_font_def_aram_flag(D17A8[bB4].samples[bB0].font);
        req.x = 480.0f;
        req.y = 200.0f;
        req.z = 1.0f;
        req.scaleX = 1.0f;
        req.scaleY = 1.0f;
        req.u1 = 0.0f;
        req.v1 = 0.0f;
        req.u2 = 1.0f;
        req.v2 = 1.0f;
        req.rotation = 0;
        req.opacity = 1.0f;
        req.unk30 = -1;
        req.flags = 0xA;
        req.mulColor = -1;
        req.addColor = 0;
        draw_sprite_draw_request(&req);
    }
}

void fn_8_5A14(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        bB8 = 3;
        bBA = -1;
        bBC = 0;
        bBD = 0;
        bBE = 0;
        bC0 = 0;
        sub_mode_request = 0x9E;
    }
}

void fn_8_5A78(void)
{
    int prevB;
    int prev;
    int v4;
    int tr5;
    int v0;
    int prevC;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        prev = bB8;
        if (PAD[4].button & 2) {
            if (++bB8 > 0x90) {
                bB8 = 0;
            }
        }
        if (PAD[4].button & 1) {
            if (--bB8 < 0) {
                bB8 = 0x90;
            }
        }
        if (prev != bB8) {
            bC0 = FONT_DEFINITIONS[bB8].bmpId == -1 ? 0 : FONT_DEFINITIONS[bB8].firstChar;
            bBE = 0;
            if (FONT_DEFINITIONS[bB8].bmpId == -1) {
                bBA = bB8;
            }
        }
        if (FONT_DEFINITIONS[bB8].bmpId == -1) {
            prevB = bBE;
            tr5 = g_related_to_aram_font_pages[bB8] / 128;
            if (PAD[4].button & 4) {
                if (++bBE > tr5) {
                    bBE = 0;
                }
            }
            if (PAD[4].button & 8) {
                if (--bBE < 0) {
                    bBE = tr5;
                }
            }
            if (prevB != bBE && bC0 / 128 != bBE) {
                bC0 = bBE << 7;
            }
        }
        prevC = bC0;
        if (FONT_DEFINITIONS[bB8].bmpId == -1) {
            v0 = 0;
            v4 = g_related_to_aram_font_pages[bB8];
        } else {
            v0 = FONT_DEFINITIONS[bB8].firstChar;
            v4 = FONT_DEFINITIONS[bB8].lastChar + 1;
        }
        if (analog_inputs[0][4] & 0x20) {
            if (++bC0 > v4 - 1) {
                bC0 = v0;
            }
        }
        if (analog_inputs[0][4] & 0x10) {
            if (--bC0 < v0) {
                bC0 = v4 - 1;
            }
        }
        if (analog_inputs[0][4] & 0x40) {
            bC0 = bC0 + 0x10;
            if (bC0 > v4 - 1) {
                bC0 = bC0 % 16;
            }
        }
        if (analog_inputs[0][4] & 0x80) {
            bC0 = bC0 - 0x10;
            if (bC0 < v0) {
                bC0 = bC0 % 16 + ((v4 - 1) / 16 + 1) * 16;
            }
            if (bC0 > v4 - 1) {
                bC0 = bC0 - 0x10;
            }
        }
        if (prevC != bC0 && bC0 / 128 != bBE) {
            bBE = bC0 / 128;
        }
        if (FONT_DEFINITIONS[bB8].bmpId == -1) {
            if (PAD[2].button & 0x400) {
                bBC = bBC ^ 1;
            }
            if (PAD[2].button & 0x800) {
                if (++bBD == 6) {
                    bBD = 0;
                }
            }
        }
    }
}

void fn_8_5EF0(void)
{
    u8 buf3[4];
    u8 buf2[4];
    struct FontParams *tr28;
    int v27;
    int v26;
    u16 *v25;
    char *v6;

    tr28 = &FONT_DEFINITIONS[bB8];
    textdraw_reset();
    textdraw_set_font(2);
    textdraw_set_pos_for_aram_font_test(1.0f, 1.0f);
    textdraw_print("FONT TEST\n\n");
    if (tr28->bmpId == -1) {
        v6 = "ARAM";
    } else {
        v6 = "NORMAL";
    }
    textdraw_printf("font:%3d/%3d              type :%s FONT\n", bB8, 0x90, v6);
    if (tr28->bmpId == -1) {
        textdraw_printf("name:%-20s style:%d/%d %s\n", FONT_NAME_LIST[bB8], bBD, 5, FONT_STYLE_NAME_LIST[bBD]);
    } else {
        textdraw_printf("name:%-20s\n", FONT_NAME_LIST[bB8]);
    }
    textdraw_print("\n");
    textdraw_print("\035\034:font select\n");
    if (tr28->bmpId == -1) {
        textdraw_print("\036\037:page next/prev\n");
    }
    textdraw_set_mul_color(0xFFFF00);
    textdraw_print("3D");
    textdraw_set_mul_color(0xFFFFFF);
    textdraw_print(":charactor select\n");
    if (tr28->bmpId == -1) {
        textdraw_print("X :fixed/proportional\n");
        textdraw_print("Y :style change\n");
    }
    textdraw_set_font(bB8);
    textdraw_set_pos(320.0f - tr28->unk34 * (tr28->spaceWidth * 16) / 2.0f, 150.0f);
    if (bBC != 0) {
        textdraw_set_flags(0x80000000);
    }
    textdraw_set_font_style(bBD);
    if (tr28->bmpId == -1) {
        buf3[2] = 0;
        v25 = tr28->string;
        v27 = 0;
        v26 = 0;
        while (*v25 != 0) {
            if (v27 >= bBE << 7) {
                if (v27 >= (bBE << 7) + 0x80) {
                    break;
                }
                buf3[0] = (u8)((*v25 >> 8) & 0xFF);
                buf3[1] = (u8)(*v25 & 0xFF);
                if (bC0 == v27) {
                    textdraw_set_mul_color(0xFFFF00);
                }
                if (tr28->pageIdx[v27] == -1) {
                    textdraw_set_font(2);
                    textdraw_print("*");
                    textdraw_set_font(bB8);
                } else {
                    textdraw_print((char *)buf3);
                }
                if (v27 % 16 == 0xF) {
                    textdraw_print("\n");
                }
                if (bC0 == v27) {
                    textdraw_set_mul_color(0xFFFFFF);
                }
            }
            v27++;
            v25++;
            v26++;
        }
        if (tr28->pageIdx[bC0] != -1) {
            buf3[0] = (u8)((tr28->string[bC0] >> 8) & 0xFF);
            buf3[1] = (u8)(tr28->string[bC0] & 0xFF);
            textdraw_set_pos(12.0f, 150.0f);
            textdraw_set_scale(2.0f, 2.0f);
            textdraw_print((char *)buf3);
            textdraw_set_pos(12.0f, 150.0f);
            textdraw_set_scale(2.0f, 2.0f);
            textdraw_set_mul_color(0x6060);
            textdraw_set_depth(100.0f);
            textdraw_print("\201\241");
            textdraw_set_mul_color(0xFFFFFF);
            textdraw_set_depth(0.1f);
        }
        textdraw_set_font(2);
        textdraw_clear_flags(0x80000000);
        textdraw_set_pos(12.0f, 150 + 12 + tr28->lineHeight * 2);
        textdraw_set_scale(1.0f, 1.0f);
        textdraw_printf("%X\n", tr28->string[bC0]);
        if (tr28->pageIdx[bC0] == -1) {
            textdraw_print("is no load.\n");
        } else {
            textdraw_printf("aram:%d\n", tr28->pageIdx[bC0]);
        }
        textdraw_printf("\npage:%d/%d\n", bBE, g_related_to_aram_font_pages[bB8] / 128);
        return;
    }
    buf2[1] = 0;
    v27 = tr28->firstChar;
    for (v26 = 0; v27 < tr28->lastChar + 1; v26++) {
        if (bC0 == v27) {
            textdraw_set_mul_color(0xFFFF00);
        }
        if (v27 == 0) {
            textdraw_put_char(0);
            textdraw_print(" ");
        } else if (v27 == 0xA) {
            textdraw_put_char(0xA);
            textdraw_print(" ");
        } else {
            buf2[0] = v27;
            textdraw_print((char *)buf2);
        }
        if (v26 % 16 == 0xF) {
            textdraw_print("\n");
        }
        textdraw_set_mul_color(0xFFFFFF);
        textdraw_set_add_color(0);
        v27++;
    }
    textdraw_clear_flags(0x80000000);
    buf2[0] = bC0;
    textdraw_set_pos(12.0f, 150.0f);
    textdraw_set_scale(2.0f, 2.0f);
    textdraw_print((char *)buf2);
    textdraw_set_font(2);
    textdraw_set_pos(12.0f, 150 + 12 + tr28->lineHeight * 2);
    textdraw_set_scale(1.0f, 1.0f);
    textdraw_printf("%X", bC0);
}



void fn_8_64D4(struct Ape *arg0, s32 arg1)
{
    float vf3;
    struct Ape *tr5;
    s32 v6;
    s32 v5;

    v6 = (arg1 == 3);
    v6 |= (bC4 == 1);
    v6 |= (main_mode != 3);
    v5 = 0;
    if ((sub_mode != 0x8A) && (sub_mode != 0x8B)) {
        v5 = 1;
    }
    v6 |= v5;
    if (v6 != 0) {
        fn_1_D1F28(arg0);
        if (arg1 != 3) {
            fn_1_D292C();
        }
        return;
    }
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        if ((u32) arg0->unk29C != 0) {
            vf3 = 0.7f;
            tr5 = bC8[(u32) arg0->unk29C - 1];
            if ((u32) (arg0->unk29C & 1) != 0) {
                vf3 *= -1.0f;
            }
            arg0->pos = tr5->pos;
            arg0->pos.x += vf3 * (u32) arg0->unk29C;
            arg0->rot = tr5->rot;
        }
        fn_1_D2510(arg0);
    }
}

static struct ModelParam D1924[8] = {
    { 26, 2.0f, 15.0f, 0.1f, 4, 2 },
    { 21, 2.0f, 15.0f, 0.1f, 5, 3 },
    { 27, 1.0f, 8.0f, 0.05f, 0, 0 },
    { 22, 1.0f, 8.0f, 0.05f, 0, 0 },
    { 24, 5.0f, 70.000000f, 0.05f, 6, 0 },
    { 19, 5.0f, 70.000000f, 0.05f, 7, 0 },
    { 23, 2.0f, 10.000000f, 0.05f, 0, 0 },
    { 18, 2.0f, 10.000000f, 0.05f, 0, 0 },
};

char *motape_skel(void) {
    if (b108 < 0) {
        b108 = lbl_1_bss_C8AE8->skelCount - 1;
    } else if ((u32) b108 >= lbl_1_bss_C8AE8->skelCount) {
        b108 = 0;
    }
    return lbl_1_bss_C8AE8->skels[b108].name;
}
char *motape_model(void) {
    if (b10C < 0) {
        b10C = lbl_1_bss_C8AE8->modelCount - 1;
    } else if ((u32) b10C >= lbl_1_bss_C8AE8->modelCount) {
        b10C = 0;
    }
    return lbl_1_bss_C8AE8->models[b10C].name;
}
void motape_spawn(void) {
    if (b110 < 0x10) {
        fn_1_D1F0C(b10C, 0);
        bC8[b110] = fn_1_D1F48(motape_skel(), motape_model());
        bC8[b110]->charaId = b10C;
        bC8[b110]->unk29C = b110;
        b114[b110] = fn_1_D288C(fn_8_64D4, bC8[b110], 5);
        b154 = (Vec){0.0f, 0.0f, 1.0f};
        mtxa_from_identity();
        mtxa_rotate_y(0x4000);
        mtxa_to_quat(&bC8[b110]->rot);
        if (b110 != 0) {
            float vf3 = 0.7f;
            if (b110 & 1) {
                vf3 *= -1.0f;
            }
            bC8[b110]->pos = bC8[b110 - 1]->pos;
            bC8[b110]->pos.x += vf3 * b110;
        } else if (b10C == 3) {
            bC8[b110]->pos = (Vec){0.0f, 0.4f, 0.0f};
        } else {
            bC8[b110]->pos = (Vec){0.0f, 0.38f, 0.0f};
        }
        fn_1_D1FBC(bC8[b110], D1924, 8);
        b110 = b110 + 1;
    }
}

void fn_8_6678(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        event_init(0x10);
        enable_cameras_for_active_players();
        camera_set_state(3);
        g_current_camera->eye.x = 0.0f;
        g_current_camera->eye.y = 1.0f;
        g_current_camera->eye.z = 1.5f;
        g_current_camera->lookAt.x = 0.0f;
        g_current_camera->lookAt.y = 0.5f;
        g_current_camera->lookAt.z = 0.0f;
        b10C = 0;
        b108 = 0;
        bC4 = 0;
        b110 = 0;
        motape_spawn();
        b160 = 1.0f;
        modeCtrl.unk10 = 0;
        sub_mode_request = 0x8B;
        call_g_avdisp_set_ambient(0.3f, 0.3f, 0.3f);
    }
}

static char *D1A1C[4] = {
    "High Level", "Middle Level", "Low Level", "More Low Level",
};

static char *D1A3C[4] = {
    "APE", "GAL", "KID", "GOR",
};

void fn_8_6AD4(void)
{
    Vec sp3C;
    s16 v0;
    s32 tr0;
    s32 tr3;
    s32 v27;
    struct Ape *tr29;
    s32 v28;
    u32 v4;
    int stickX;
    int stickY;
    int substickX;
    int substickY;

    tr29 = bC8[0];
    if (bC4 != 0) {
        tr3 = fn_8_16B04();
        bC4 = tr3;
        if (tr3 == 0) {
            g_set_up_debugtext_buffer();
            camera_set_state(3);
            g_current_camera->eye.x = 0.0f;
            g_current_camera->eye.y = 1.0f;
            g_current_camera->eye.z = 1.5f;
            g_current_camera->lookAt.x = 0.0f;
            g_current_camera->lookAt.y = 0.5f;
            g_current_camera->lookAt.z = 0.0f;
            b110 = 0;
            motape_spawn();
        }
        return;
    }
    if (PAD_ANA(8)) {
        if (--modeCtrl.unk10 < 0) {
            modeCtrl.unk10 = 7;
        }
    } else if (PAD_ANA(4)) {
        if (++modeCtrl.unk10 >= 8) {
            modeCtrl.unk10 = 0;
        }
    }
    debug_textdraw_set_pos(2, 1);
    for (v27 = 0; v27 < 8; v27++) {
        char *colors[4] = {"red(default)", "blue", "yellow", "green"};
        if (v27 == modeCtrl.unk10) {
            debug_textdraw_set_color(0x33);
            fn_1_3E100(-1, 0);
            debug_textdraw_put_buf_1("*");
        } else {
            debug_textdraw_set_color(0xFF);
        }
    }
    debug_textdraw_set_color(0xFF);
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        mtxa_from_mtxb();
        stickX = PAD[0].stickX;
        stickY = PAD[0].stickY;
        substickX = PAD[0].substickX;
        substickY = PAD[0].substickY;
        sp3C.x = ((0.0008 * stickX) - (0.0008 * substickX));
        sp3C.y = (0.0008 * stickY);
        sp3C.z = (0.001 * substickY);
        mtxa_rigid_inv_tf_vec(&sp3C, &sp3C);
        PSVECAdd(&sp3C, &g_current_camera->eye, &g_current_camera->eye);
        if (PAD[2].button & 0x800) {
            camera_set_state(3);
            g_current_camera->eye.x = 0.0f;
            g_current_camera->eye.y = 1.0f;
            g_current_camera->eye.z = 1.5f;
            g_current_camera->lookAt.x = 0.0f;
            g_current_camera->lookAt.y = 0.5f;
            g_current_camera->lookAt.z = 0.0f;
        }
        switch (modeCtrl.unk10) {
        case 0:
            if (PAD_ANA(1)) {
                do {
                    v28--;
                    if (v28 < 1) {
                        v28 = lbl_1_bss_C8AEC;
                    }
                    v0 = v28 - 1;
                    if (v0 < 1) {
                        v0 = (s16) lbl_1_bss_C8AEC;
                    }
                } while (lbl_1_bss_700B0[v0].unk2 != 0);
            } else if (PAD_ANA(2)) {
                do {
                    v28++;
                    if (v28 > lbl_1_bss_C8AEC) {
                        v28 = 1;
                    }
                    v0 = v28 - 1;
                    if (v0 < 1) {
                        v0 = (s16) lbl_1_bss_C8AEC;
                    }
                } while (lbl_1_bss_700B0[v0].unk2 != 0);
            }
            break;
        case 1:
        case 2:
            break;
        case 4:
            if (PAD_ANA(1)) {
                v4 = tr29->lod;
                v4--;
                fn_1_D1FB8(tr29, v4);
            } else if (PAD_ANA(2)) {
                v4 = tr29->lod;
                if (++v4 >= 4) {
                    v4 = 0;
                }
                fn_1_D1FB8(tr29, v4);
            }
            break;
        case 3:
            if (PAD[4].button & 1) {
                tr0 = tr29->colorIndex - 1;
                tr29->colorIndex = tr0;
                if (tr0 < 0) {
                    tr29->colorIndex = 3;
                }
            } else if (PAD[4].button & 2) {
                tr0 = tr29->colorIndex + 1;
                tr29->colorIndex = tr0;
                if (tr0 > 3) {
                    tr29->colorIndex = 0;
                }
            }
            break;
        case 5:
            if (PAD_ANA(1)) {
                b160 = (b160 - 0.01);
                if (b160 < 0.0) {
                    b160 = 0.0f;
                }
            } else if (PAD_ANA(2)) {
                b160 += 0.01;
                if (b160 > 3.0) {
                    b160 = 3.0f;
                }
            }
            break;
        case 6:
        case 7:
            break;
        }
        if (PAD[2].button & 0x100) {
            bC4 = 1;
            fn_1_D29E0(b114[0]);
            fn_8_165C4();
        }
    }
}

void fn_8_7608(void)
{
    fn_8_175E0();
    sub_mode_request = 0x9C;
}

void fn_8_7638(void)
{
    fn_8_177C4();
}

void fn_8_7658(void)
{
    main_mode_request = 0;
    sub_mode_request = 0xF;
}

void fn_8_767C(void)
{
    switch (sub_mode) {
    case 0x8D:
    case 0x8E:
        set_sub_mode_destination(fn_8_767C);
        return;
    }
    fn_80065BA8(b164);
    g_something_freeing_heap_2(b168);
    OSFreeToHeap(currentHeap, b16C);
}

static char *D1AF0[10] = {
    "Material Change", "Material Freeze",
    "Blend", "Texture Only",
    "Constant", "Gouraud",
    "Double Side", "Single Side",
    "Fog Disable", "Fog Enable",
};

void fn_8_76FC(void)
{
    u32 v5;

    b168 = g_load_tpl("boy_h/boy_h.tpl");
    b164 = g_load_raw_gma("boy_kao.gcm", b168);
    b170 = fn_8006991C(b164);
    b174 = fn_80069870(b164);
    b16C = alloc_from_heap_or_panic(currentHeap, ((*b170 * 0xC) + 0x1F) & 0xFFFFFFE0, "test_mode.c", 0xFB3);
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(2);
    set_sub_mode_destination(fn_8_767C);
    sub_mode_request = 0x8E;
    call_g_avdisp_set_ambient(0.3f, 0.3f, 0.3f);
    b178 = 0;
    for (v5 = 0; v5 < 5; v5++) {
        b17C[v5] = 0;
    }
}

void fn_8_7820(void)
{
    u32 v29;

    debug_textdraw_set_pos(2, 2);
    debug_textdraw_vsprintf_buf_1("Effective Model Test\n");
    debug_textdraw_set_pos(4, 5);
    if (PAD[4].button & 8) {
        b178 = (b178 + 4);
    }
    if (PAD[4].button & 4) {
        b178 = (b178 + 1);
    }
    b178 = (u32) b178 % 5;
    if (PAD[2].button & 0x100) {
        b17C[b178] ^= 1;
        switch (b178) {         /* irregular */
        case 0:
            break;
        case 1:
            fn_800699B4(b164, 8, 0);
            break;
        case 2:
            fn_800699B4(b164, 1, 0);
            break;
        case 3:
            fn_800699B4(b164, 2, 0);
            break;
        case 4:
            fn_800699B4(b164, 4, 0);
            break;
        }
    }
    for (v29 = 0; v29 < 5; v29++) {
        if ((int) v29 == b178) {
            debug_textdraw_set_color(0x33);
            debug_textdraw_put_buf_1("-> ");
        } else {
            debug_textdraw_set_color(0xFF);
            debug_textdraw_put_buf_1("   ");
        }
        debug_textdraw_put_buf_1(D1AF0[v29 * 2 + b17C[v29]]);
        debug_textdraw_put_buf_1("\n");
    }
}

void fn_8_7A0C(void)
{
    float vf31;
    u32 v29;

    mtxa_from_mtxb();
    mtxa_translate(&g_current_camera->lookAt);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    mtxa_scale_s(0.5f);
    fn_8_7CC8();
    mtxa_from_mtxb();
    mtxa_translate_xyz(0.0f, 0.1f, -1.0f);
    mtxa_rotate_x(-0x4000);
    mtxa_rotate_z(-0x4000);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
    for (v29 = 0; v29 < *b170; v29++) {
        b16C[v29] = b174[v29].pos;
        switch ((int) v29 % 3) {
        case 0:
            vf31 = (0.02 * math_sin((s16) (frame_counter << 9)));
            break;
        case 1:
            vf31 = 0.03f * math_sin((s16) (frame_counter << 8));
            break;
        case 2:
            vf31 = 0.02f * math_sin((s16) (frame_counter << 0xA));
            break;
        }
        b174[v29].pos.x += vf31 * b174[v29].nrm.x;
        b174[v29].pos.y += vf31 * b174[v29].nrm.y;
        b174[v29].pos.z += vf31 * b174[v29].nrm.z;
    }
    avdisp_set_fog_params(2, 2.0f, 15.0f);
    avdisp_set_fog_color(0, 0x59, 0x69);
    g_set_something3(1);
    avdisp_draw_model_unculled_sort_auto(b164);
    fn_1_E2D20();
    for (v29 = 0; v29 < *b170; v29++) {
        b174[v29].pos = b16C[v29];
    }
}

static char *D1B80[5] = {
    "Normal", "All Disp", "Cat Next", "Cat Prev", "Load",
};

static char *D1ED4[204] = {
    "init", "common",
    "init", "chara",
    "bg", "bg_jun",
    "bg", "bg_wat",
    "bg", "bg_nig",
    "bg", "bg_sun",
    "bg", "bg_spa",
    "bg", "bg_snd",
    "bg", "bg_ice",
    "bg", "bg_stm",
    "bg", "bg_bns",
    "st131", "st131",
    "st132", "st132",
    "st133", "st133",
    "st134", "st134",
    "st135", "st135",
    "st136", "st136",
    "st137", "st137",
    "st138", "st138",
    "st139", "st139",
    "st140", "st140",
    "st141", "st141",
    "st142", "st142",
    "st143", "st143",
    "st144", "st144",
    "st145", "st145",
    "st146", "st146",
    "st147", "st147",
    "st148", "st148",
    "st149", "st149",
    "st150", "st150",
    "st151", "st151",
    "st152", "st152",
    "st153", "st153",
    "st154", "st154",
    "st155", "st155",
    "st156", "st156",
    "st157", "st157",
    "st158", "st158",
    "st159", "st159",
    "st160", "st160",
    "st161", "st161",
    "st162", "st162",
    "st163", "st163",
    "st164", "st164",
    "st165", "st165",
    "st166", "st166",
    "st167", "st167",
    "st168", "st168",
    "st169", "st169",
    "st170", "st170",
    "st171", "st171",
    "st172", "st172",
    "st173", "st173",
    "st174", "st174",
    "st175", "st175",
    "st176", "st176",
    "st177", "st177",
    "st178", "st178",
    "st179", "st179",
    "st180", "st180",
    "st181", "st181",
    "st182", "st182",
    "st183", "st183",
    "st184", "st184",
    "st185", "st185",
    "st186", "st186",
    "st187", "st187",
    "st188", "st188",
    "st189", "st189",
    "st180", "st190",
    "st191", "st191",
    "st192", "st192",
    "st193", "st193",
    "st194", "st194",
    "st195", "st195",
    "st196", "st196",
    "st197", "st197",
    "st198", "st198",
    "st199", "st199",
    "st200", "st200",
    "boy_h", "boy_h",
    "boy_l", "boy_l",
    "gal_h", "gal_h",
    "gal_l", "gal_l",
    "kid_h", "kid_h",
    "kid_l", "kid_l",
    "init", "m_boa2",
    "init", "m_dog2",
    "init", "m_gun2_1",
    "init", "m_gun2_2",
    "init", "m_gun2_3",
    "init", "m_fut2",
    "init", "m_race",
    "init", "m_fight",
    "init", "m_pilot",
    "init", "m_bowl",
    "init", "m_golf",
    "init", "m_bill2",
    "init", "m_bowl2",
    "init", "m_tar2",
    "init", "test",
};

void model_test_unused(void)
{
    debug_textdraw_vsprintf_buf_1("%s\n");
    debug_textdraw_vsprintf_buf_1(".tpl");
    debug_textdraw_vsprintf_buf_1(".gma");
    debug_textdraw_vsprintf_buf_1("..");
    debug_textdraw_vsprintf_buf_1("%s/%s.tpl");
    debug_textdraw_vsprintf_buf_1("%s/%s.gma");
    debug_textdraw_vsprintf_buf_1("Category [%s] Please Load");
    debug_textdraw_vsprintf_buf_1("Category [%s] no data\n");
    debug_textdraw_vsprintf_buf_1("Category [%s] %d/%d");
    debug_textdraw_vsprintf_buf_1("name: %s\nrad: %f\n");
    debug_textdraw_vsprintf_buf_1("Time: %d\n");
    b190 = 0;
    b194 = 0;
    b198 = NULL;
    b1A0 = 0;
    b1A4 = 0;
    b1A8 = 0;
    b1AC = 0;
    b1AE = 0;
    s_sel[0] = 0;
    b1B6 = 0;
    b1B8 = 0;
    b1BC = 0.0f;
}

void fn_8_7CC8(void)
{
    GXColor mat = {0xFF, 0xFF, 0xFF, 0x32};
    GXColor amb = {0x64, 0x64, 0x64, 0x32};

    GXSetChanMatColor(0, mat);
    GXSetChanAmbColor(0, amb);
    opti_GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumChans_cached(1);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
    GXSetNumTexGens_cached(0);
    GXSetNumTevStages_cached(1);
    mtxa_from_mtxb_translate(&g_current_camera->lookAt);
    mtxa_scale_s(0.4f);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXSetCurrentMtx(0);
    GXDrawSphere(8, 8);
}

static char *D22C4[8] = {
    "common", "bg", "stage", "chara",
    "minigame", "tennis", "baseball", "test",
};

static struct ModelEntry D3208[554] = {
    { 0, "init", "common" },
    { 0, "init", "commend" },
    { 0, "init", "roll" },
    { 0, "init", "pattern" },
    { 0, "init", "pattern_spk" },
    { 0, "init", "pattern_gem" },
    { 0, "init", "pattern_har" },
    { 0, "init", "pattern_spkb" },
    { 0, "init", "efcmdl" },
    { 1, "bg", "bg_lav2" },
    { 1, "bg", "bg_jun2" },
    { 1, "bg", "bg_wat2" },
    { 1, "bg", "bg_par2" },
    { 1, "bg", "bg_pot2" },
    { 1, "bg", "bg_spa2" },
    { 1, "bg", "bg_ele2" },
    { 1, "bg", "bg_gea2" },
    { 1, "bg", "bg_bub2" },
    { 1, "bg", "bg_bns2" },
    { 1, "bg", "bg_rac2" },
    { 1, "bg", "bg_fig2" },
    { 1, "bg", "bg_tar2" },
    { 1, "bg", "bg_gol2" },
    { 1, "bg", "bg_bow2" },
    { 1, "bg", "bg_bil2" },
    { 1, "bg", "bg_boa2" },
    { 1, "bg", "bg_dog2" },
    { 1, "bg", "bg_gun2" },
    { 1, "bg", "bg_fut2" },
    { 1, "bg", "bg_wha2" },
    { 1, "bg", "bg_au_bub2" },
    { 1, "bg", "bg_au_par2" },
    { 1, "bg", "bg_au_gea2" },
    { 1, "bg", "bg_au_wat2" },
    { 1, "bg", "bg_au_tar2" },
    { 1, "bg", "bg_jun" },
    { 1, "bg", "bg_wat" },
    { 1, "bg", "bg_nig" },
    { 1, "bg", "bg_sun" },
    { 1, "bg", "bg_spa" },
    { 1, "bg", "bg_snd" },
    { 1, "bg", "bg_ice" },
    { 1, "bg", "bg_stm" },
    { 1, "bg", "bg_bns" },
    { 1, "bg", "bg_rac" },
    { 1, "bg", "bg_fig" },
    { 1, "bg", "bg_pil" },
    { 1, "bg", "bg_gol" },
    { 1, "bg", "bg_bow" },
    { 1, "bg", "bg_bil" },
    { 1, "bg", "bg_mst" },
    { 1, "bg", "bg_ending" },
    { 2, "stage", "st001" },
    { 2, "stage", "st002" },
    { 2, "stage", "st003" },
    { 2, "stage", "st004" },
    { 2, "stage", "st005" },
    { 2, "stage", "st006" },
    { 2, "stage", "st007" },
    { 2, "stage", "st008" },
    { 2, "stage", "st009" },
    { 2, "stage", "st010" },
    { 2, "stage", "st011" },
    { 2, "stage", "st012" },
    { 2, "stage", "st013" },
    { 2, "stage", "st014" },
    { 2, "stage", "st015" },
    { 2, "stage", "st016" },
    { 2, "stage", "st017" },
    { 2, "stage", "st018" },
    { 2, "stage", "st019" },
    { 2, "stage", "st020" },
    { 2, "stage", "st021" },
    { 2, "stage", "st022" },
    { 2, "stage", "st023" },
    { 2, "stage", "st024" },
    { 2, "stage", "st025" },
    { 2, "stage", "st026" },
    { 2, "stage", "st027" },
    { 2, "stage", "st028" },
    { 2, "stage", "st029" },
    { 2, "stage", "st030" },
    { 2, "stage", "st031" },
    { 2, "stage", "st032" },
    { 2, "stage", "st033" },
    { 2, "stage", "st034" },
    { 2, "stage", "st035" },
    { 2, "stage", "st036" },
    { 2, "stage", "st037" },
    { 2, "stage", "st038" },
    { 2, "stage", "st039" },
    { 2, "stage", "st040" },
    { 2, "stage", "st041" },
    { 2, "stage", "st042" },
    { 2, "stage", "st043" },
    { 2, "stage", "st044" },
    { 2, "stage", "st045" },
    { 2, "stage", "st046" },
    { 2, "stage", "st047" },
    { 2, "stage", "st048" },
    { 2, "stage", "st049" },
    { 2, "stage", "st050" },
    { 2, "stage", "st051" },
    { 2, "stage", "st052" },
    { 2, "stage", "st053" },
    { 2, "stage", "st054" },
    { 2, "stage", "st055" },
    { 2, "stage", "st056" },
    { 2, "stage", "st057" },
    { 2, "stage", "st058" },
    { 2, "stage", "st059" },
    { 2, "stage", "st060" },
    { 2, "stage", "st061" },
    { 2, "stage", "st062" },
    { 2, "stage", "st063" },
    { 2, "stage", "st064" },
    { 2, "stage", "st065" },
    { 2, "stage", "st066" },
    { 2, "stage", "st067" },
    { 2, "stage", "st068" },
    { 2, "stage", "st069" },
    { 2, "stage", "st070" },
    { 2, "stage", "st071" },
    { 2, "stage", "st072" },
    { 2, "stage", "st073" },
    { 2, "stage", "st074" },
    { 2, "stage", "st075" },
    { 2, "stage", "st076" },
    { 2, "stage", "st077" },
    { 2, "stage", "st078" },
    { 2, "stage", "st079" },
    { 2, "stage", "st080" },
    { 2, "stage", "st081" },
    { 2, "stage", "st082" },
    { 2, "stage", "st083" },
    { 2, "stage", "st084" },
    { 2, "stage", "st085" },
    { 2, "stage", "st086" },
    { 2, "stage", "st087" },
    { 2, "stage", "st088" },
    { 2, "stage", "st089" },
    { 2, "stage", "st090" },
    { 2, "stage", "st091" },
    { 2, "stage", "st092" },
    { 2, "stage", "st093" },
    { 2, "stage", "st094" },
    { 2, "stage", "st095" },
    { 2, "stage", "st096" },
    { 2, "stage", "st097" },
    { 2, "stage", "st098" },
    { 2, "stage", "st099" },
    { 2, "stage", "st100" },
    { 2, "stage", "st101" },
    { 2, "stage", "st102" },
    { 2, "stage", "st103" },
    { 2, "stage", "st104" },
    { 2, "stage", "st105" },
    { 2, "stage", "st106" },
    { 2, "stage", "st107" },
    { 2, "stage", "st108" },
    { 2, "stage", "st109" },
    { 2, "stage", "st110" },
    { 2, "stage", "st111" },
    { 2, "stage", "st112" },
    { 2, "stage", "st113" },
    { 2, "stage", "st114" },
    { 2, "stage", "st115" },
    { 2, "stage", "st116" },
    { 2, "stage", "st117" },
    { 2, "stage", "st118" },
    { 2, "stage", "st119" },
    { 2, "stage", "st120" },
    { 2, "stage", "st121" },
    { 2, "stage", "st122" },
    { 2, "stage", "st123" },
    { 2, "stage", "st124" },
    { 2, "stage", "st125" },
    { 2, "stage", "st126" },
    { 2, "stage", "st127" },
    { 2, "stage", "st128" },
    { 2, "stage", "st129" },
    { 2, "stage", "st130" },
    { 2, "stage", "st131" },
    { 2, "stage", "st132" },
    { 2, "stage", "st133" },
    { 2, "stage", "st134" },
    { 2, "stage", "st135" },
    { 2, "stage", "st136" },
    { 2, "stage", "st137" },
    { 2, "stage", "st138" },
    { 2, "stage", "st139" },
    { 2, "stage", "st140" },
    { 2, "stage", "st141" },
    { 2, "stage", "st142" },
    { 2, "stage", "st143" },
    { 2, "stage", "st144" },
    { 2, "stage", "st145" },
    { 2, "stage", "st146" },
    { 2, "stage", "st147" },
    { 2, "stage", "st148" },
    { 2, "stage", "st149" },
    { 2, "stage", "st150" },
    { 2, "stage", "st151" },
    { 2, "stage", "st152" },
    { 2, "stage", "st153" },
    { 2, "stage", "st154" },
    { 2, "stage", "st155" },
    { 2, "stage", "st156" },
    { 2, "stage", "st157" },
    { 2, "stage", "st158" },
    { 2, "stage", "st159" },
    { 2, "stage", "st160" },
    { 2, "stage", "st161" },
    { 2, "stage", "st162" },
    { 2, "stage", "st163" },
    { 2, "stage", "st164" },
    { 2, "stage", "st165" },
    { 2, "stage", "st166" },
    { 2, "stage", "st167" },
    { 2, "stage", "st168" },
    { 2, "stage", "st169" },
    { 2, "stage", "st170" },
    { 2, "stage", "st171" },
    { 2, "stage", "st172" },
    { 2, "stage", "st173" },
    { 2, "stage", "st174" },
    { 2, "stage", "st175" },
    { 2, "stage", "st176" },
    { 2, "stage", "st177" },
    { 2, "stage", "st178" },
    { 2, "stage", "st179" },
    { 2, "stage", "st180" },
    { 2, "stage", "st181" },
    { 2, "stage", "st182" },
    { 2, "stage", "st183" },
    { 2, "stage", "st184" },
    { 2, "stage", "st185" },
    { 2, "stage", "st186" },
    { 2, "stage", "st187" },
    { 2, "stage", "st188" },
    { 2, "stage", "st189" },
    { 2, "stage", "st190" },
    { 2, "stage", "st191" },
    { 2, "stage", "st192" },
    { 2, "stage", "st193" },
    { 2, "stage", "st194" },
    { 2, "stage", "st195" },
    { 2, "stage", "st196" },
    { 2, "stage", "st197" },
    { 2, "stage", "st198" },
    { 2, "stage", "st199" },
    { 2, "stage", "st200" },
    { 2, "stage", "st201" },
    { 2, "stage", "st202" },
    { 2, "stage", "st203" },
    { 2, "stage", "st204" },
    { 2, "stage", "st205" },
    { 2, "stage", "st206" },
    { 2, "stage", "st207" },
    { 2, "stage", "st208" },
    { 2, "stage", "st209" },
    { 2, "stage", "st210" },
    { 2, "stage", "st211" },
    { 2, "stage", "st212" },
    { 2, "stage", "st213" },
    { 2, "stage", "st214" },
    { 2, "stage", "st215" },
    { 2, "stage", "st216" },
    { 2, "stage", "st217" },
    { 2, "stage", "st218" },
    { 2, "stage", "st219" },
    { 2, "stage", "st220" },
    { 2, "stage", "st221" },
    { 2, "stage", "st222" },
    { 2, "stage", "st223" },
    { 2, "stage", "st224" },
    { 2, "stage", "st225" },
    { 2, "stage", "st226" },
    { 2, "stage", "st227" },
    { 2, "stage", "st228" },
    { 2, "stage", "st229" },
    { 2, "stage", "st230" },
    { 2, "stage", "st231" },
    { 2, "stage", "st232" },
    { 2, "stage", "st233" },
    { 2, "stage", "st234" },
    { 2, "stage", "st235" },
    { 2, "stage", "st236" },
    { 2, "stage", "st237" },
    { 2, "stage", "st238" },
    { 2, "stage", "st239" },
    { 2, "stage", "st240" },
    { 2, "stage", "st241" },
    { 2, "stage", "st242" },
    { 2, "stage", "st243" },
    { 2, "stage", "st244" },
    { 2, "stage", "st245" },
    { 2, "stage", "st246" },
    { 2, "stage", "st247" },
    { 2, "stage", "st248" },
    { 2, "stage", "st249" },
    { 2, "stage", "st250" },
    { 2, "stage", "st251" },
    { 2, "stage", "st252" },
    { 2, "stage", "st253" },
    { 2, "stage", "st254" },
    { 2, "stage", "st255" },
    { 2, "stage", "st256" },
    { 2, "stage", "st257" },
    { 2, "stage", "st258" },
    { 2, "stage", "st259" },
    { 2, "stage", "st260" },
    { 2, "stage", "st261" },
    { 2, "stage", "st262" },
    { 2, "stage", "st263" },
    { 2, "stage", "st264" },
    { 2, "stage", "st265" },
    { 2, "stage", "st266" },
    { 2, "stage", "st267" },
    { 2, "stage", "st268" },
    { 2, "stage", "st269" },
    { 2, "stage", "st270" },
    { 2, "stage", "st271" },
    { 2, "stage", "st272" },
    { 2, "stage", "st273" },
    { 2, "stage", "st274" },
    { 2, "stage", "st275" },
    { 2, "stage", "st276" },
    { 2, "stage", "st277" },
    { 2, "stage", "st278" },
    { 2, "stage", "st279" },
    { 2, "stage", "st280" },
    { 2, "stage", "st281" },
    { 2, "stage", "st282" },
    { 2, "stage", "st283" },
    { 2, "stage", "st284" },
    { 2, "stage", "st285" },
    { 2, "stage", "st286" },
    { 2, "stage", "st287" },
    { 2, "stage", "st288" },
    { 2, "stage", "st289" },
    { 2, "stage", "st290" },
    { 2, "stage", "st291" },
    { 2, "stage", "st292" },
    { 2, "stage", "st293" },
    { 2, "stage", "st294" },
    { 2, "stage", "st295" },
    { 2, "stage", "st296" },
    { 2, "stage", "st297" },
    { 2, "stage", "st298" },
    { 2, "stage", "st299" },
    { 2, "stage", "st300" },
    { 2, "stage", "st301" },
    { 2, "stage", "st302" },
    { 2, "stage", "st303" },
    { 2, "stage", "st304" },
    { 2, "stage", "st305" },
    { 2, "stage", "st306" },
    { 2, "stage", "st307" },
    { 2, "stage", "st308" },
    { 2, "stage", "st309" },
    { 2, "stage", "st310" },
    { 2, "stage", "st311" },
    { 2, "stage", "st312" },
    { 2, "stage", "st313" },
    { 2, "stage", "st314" },
    { 2, "stage", "st315" },
    { 2, "stage", "st316" },
    { 2, "stage", "st317" },
    { 2, "stage", "st318" },
    { 2, "stage", "st319" },
    { 2, "stage", "st320" },
    { 2, "stage", "st321" },
    { 2, "stage", "st322" },
    { 2, "stage", "st323" },
    { 2, "stage", "st324" },
    { 2, "stage", "st325" },
    { 2, "stage", "st326" },
    { 2, "stage", "st327" },
    { 2, "stage", "st328" },
    { 2, "stage", "st329" },
    { 2, "stage", "st330" },
    { 2, "stage", "st331" },
    { 2, "stage", "st332" },
    { 2, "stage", "st333" },
    { 2, "stage", "st334" },
    { 2, "stage", "st335" },
    { 2, "stage", "st336" },
    { 2, "stage", "st337" },
    { 2, "stage", "st338" },
    { 2, "stage", "st339" },
    { 2, "stage", "st340" },
    { 2, "stage", "st341" },
    { 2, "stage", "st342" },
    { 2, "stage", "st343" },
    { 2, "stage", "st344" },
    { 2, "stage", "st345" },
    { 2, "stage", "st346" },
    { 2, "stage", "st347" },
    { 2, "stage", "st348" },
    { 2, "stage", "st349" },
    { 2, "stage", "st350" },
    { 2, "stage", "st351" },
    { 2, "stage", "st352" },
    { 2, "stage", "st353" },
    { 2, "stage", "st354" },
    { 2, "stage", "st355" },
    { 2, "stage", "st356" },
    { 2, "stage", "st357" },
    { 2, "stage", "st358" },
    { 2, "stage", "st359" },
    { 2, "stage", "st360" },
    { 2, "stage", "st361" },
    { 2, "stage", "st362" },
    { 2, "stage", "st363" },
    { 2, "stage", "st364" },
    { 2, "stage", "st365" },
    { 2, "stage", "st366" },
    { 2, "stage", "st367" },
    { 2, "stage", "st368" },
    { 2, "stage", "st369" },
    { 2, "stage", "st370" },
    { 2, "stage", "st371" },
    { 2, "stage", "st372" },
    { 2, "stage", "st373" },
    { 2, "stage", "st374" },
    { 2, "stage", "st375" },
    { 2, "stage", "st376" },
    { 2, "stage", "st377" },
    { 2, "stage", "st378" },
    { 2, "stage", "st379" },
    { 2, "stage", "st380" },
    { 2, "stage", "st381" },
    { 2, "stage", "st382" },
    { 2, "stage", "st383" },
    { 2, "stage", "st384" },
    { 2, "stage", "st385" },
    { 2, "stage", "st386" },
    { 2, "stage", "st387" },
    { 2, "stage", "st388" },
    { 2, "stage", "st389" },
    { 2, "stage", "st390" },
    { 2, "stage", "st391" },
    { 2, "stage", "st392" },
    { 2, "stage", "st393" },
    { 2, "stage", "st394" },
    { 2, "stage", "st395" },
    { 2, "stage", "st396" },
    { 2, "stage", "st397" },
    { 2, "stage", "st398" },
    { 2, "stage", "st399" },
    { 2, "stage", "st400" },
    { 2, "stage", "st401" },
    { 2, "stage", "st402" },
    { 2, "stage", "st403" },
    { 2, "stage", "st404" },
    { 2, "stage", "st405" },
    { 2, "stage", "st406" },
    { 2, "stage", "st407" },
    { 2, "stage", "st408" },
    { 2, "stage", "st409" },
    { 2, "stage", "st410" },
    { 2, "stage", "st411" },
    { 2, "stage", "st412" },
    { 2, "stage", "st413" },
    { 2, "stage", "st414" },
    { 2, "stage", "st415" },
    { 2, "stage", "st416" },
    { 2, "stage", "st417" },
    { 2, "stage", "st418" },
    { 2, "stage", "st419" },
    { 2, "stage", "st420" },
    { 3, "ape", "aiai_h" },
    { 3, "ape", "aiai_m" },
    { 3, "ape", "aiai_l" },
    { 3, "ape", "aiai_all" },
    { 3, "ape", "aiai_sh" },
    { 3, "ape", "aiai_sm" },
    { 3, "ape", "aiai_sl" },
    { 3, "ape", "aiai_sall" },
    { 3, "ape", "meemee_h" },
    { 3, "ape", "meemee_m" },
    { 3, "ape", "meemee_l" },
    { 3, "ape", "meemee_all" },
    { 3, "ape", "meemee_sh" },
    { 3, "ape", "meemee_sm" },
    { 3, "ape", "meemee_sl" },
    { 3, "ape", "meemee_sall" },
    { 3, "ape", "baby_h" },
    { 3, "ape", "baby_m" },
    { 3, "ape", "baby_l" },
    { 3, "ape", "baby_all" },
    { 3, "ape", "baby_sh" },
    { 3, "ape", "baby_sm" },
    { 3, "ape", "baby_sl" },
    { 3, "ape", "baby_sall" },
    { 3, "ape", "gongon_h" },
    { 3, "ape", "gongon_m" },
    { 3, "ape", "gongon_l" },
    { 3, "ape", "gongon_all" },
    { 3, "ape", "gongon_sh" },
    { 3, "ape", "gongon_sm" },
    { 3, "ape", "gongon_sl" },
    { 3, "ape", "gongon_sall" },
    { 3, "ape", "madh_h" },
    { 3, "ape", "madh_m" },
    { 3, "ape", "madh_l" },
    { 3, "ape", "kobun_h" },
    { 3, "ape", "kobun_m" },
    { 3, "ape", "kobun_l" },
    { 3, "ape", "male_h" },
    { 3, "ape", "male_m" },
    { 3, "ape", "male_l" },
    { 3, "ape", "female_h" },
    { 3, "ape", "female_m" },
    { 3, "ape", "female_l" },
    { 3, "ape", "jiji_h" },
    { 3, "ape", "jiji_m" },
    { 3, "ape", "jiji_l" },
    { 3, "ape", "baba_h" },
    { 3, "ape", "baba_m" },
    { 3, "ape", "baba_l" },
    { 3, "ape", "madspa_h" },
    { 3, "ape", "madfrg_h" },
    { 3, "ape", "madnkd_h" },
    { 3, "ape", "rob_l" },
    { 3, "ape", "rob_sl" },
    { 3, "ape", "whale_h" },
    { 3, "ape", "madcrs_h" },
    { 3, "ape", "kobspa_h" },
    { 4, "init", "m_boa2" },
    { 4, "init", "m_dog2" },
    { 4, "init", "m_gun2_1" },
    { 4, "init", "m_gun2_2" },
    { 4, "init", "m_gun2_3" },
    { 4, "init", "m_fut2" },
    { 4, "init", "m_bill2" },
    { 4, "init", "m_bowl2" },
    { 4, "init", "m_tar2" },
    { 4, "init", "m_fight2" },
    { 4, "init", "m_race" },
    { 4, "init", "m_fight" },
    { 4, "init", "m_pilot" },
    { 4, "init", "m_bowl" },
    { 4, "init", "m_golf" },
    { 5, "tennis", "bt_effect" },
    { 5, "tennis", "mdl_map_c" },
    { 5, "tennis", "mdl_map_g" },
    { 5, "tennis", "mdl_map_h" },
    { 5, "tennis", "mdl_tennis" },
    { 6, "stage", "bball_obj" },
    { 6, "stage", "mdl_banana_st" },
    { 6, "stage", "mdl_monkey_dm" },
    { 7, "init", "test" },
};

static inline int model_count(int type)
{
    u32 i;
    int n = 0;

    for (i = 0; i < 0x22A; i++)
    {
        if (type == D3208[i].type)
            n++;
    }
    return n;
}

static inline char *model_name(int type, int idx)
{
    u32 i;
    int n = 0;

    for (i = 0; i < 0x22A; i++)
    {
        if (type == D3208[i].type)
            n++;
        if (idx == n)
            return D3208[i].name;
    }
    return NULL;
}

void fn_8_7DDC(void)
{
    int i;
    struct GMAModel *model;

    if (b1AE == 0) {
        if (PAD_REPEAT(8)) {
            s_sel[b1AE]--;
            if (s_sel[b1AE] < 0) {
                s_sel[b1AE] = 7;
            }
        }
        if (PAD_REPEAT(4)) {
            s_sel[b1AE]++;
            if ((u32) s_sel[b1AE] >= 8) {
                s_sel[b1AE] = 0;
            }
        }
    }
    if (b1AE == 1) {
        if (PAD_REPEAT(8)) {
            s_sel[b1AE]--;
            if (s_sel[b1AE] < 0) {
                s_sel[b1AE] = model_count(s_sel[0]) - 1;
            }
        }
        if (PAD_REPEAT(4)) {
            s_sel[b1AE]++;
            if (s_sel[b1AE] >= model_count(s_sel[0])) {
                s_sel[b1AE] = 0;
            }
        }
    }
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_set_color(0xF3);
    debug_textdraw_vsprintf_buf_1("[ Category ]");
    debug_textdraw_set_color(0xFF);
    if (b1AE < 2) {
        debug_textdraw_set_pos(1, 0x22);
        debug_textdraw_vsprintf_buf_1("A    : Select\n");
        debug_textdraw_vsprintf_buf_1("B    : Cancel\n");
        debug_textdraw_vsprintf_buf_1("PAD\036\037: Category Select\n");
    } else {
        debug_textdraw_set_pos(1, 0x22);
        debug_textdraw_vsprintf_buf_1("B    : Category Select\n");
        debug_textdraw_vsprintf_buf_1("PAD\035\034: Model Select");
    }
    if (b1A0 == 0) {
        if (b1B6 != 0) {
            debug_textdraw_set_pos(0x22, 0x21);
            debug_textdraw_vsprintf_buf_1("X  : ");
            debug_textdraw_set_color(0x33);
            debug_textdraw_vsprintf_buf_1("Used Tex View");
            debug_textdraw_set_color(0xFF);
        } else {
            debug_textdraw_set_pos(0x22, 0x21);
            debug_textdraw_vsprintf_buf_1("X  : Used Tex View");
        }
    }
    if (b1B8 != 0) {
        debug_textdraw_set_pos(0x22, 0x22);
        debug_textdraw_vsprintf_buf_1("Z  : ");
        debug_textdraw_set_color(0x33);
        debug_textdraw_vsprintf_buf_1("Fix Scale");
        debug_textdraw_set_color(0xFF);
    } else {
        debug_textdraw_set_pos(0x22, 0x22);
        debug_textdraw_vsprintf_buf_1("Z  : Variable Scale");
    }
    if (b1A0 != 0) {
        debug_textdraw_set_pos(0x22, 0x23);
        debug_textdraw_set_color(0x33);
        debug_textdraw_vsprintf_buf_1("    All Disp Mode\n");
        debug_textdraw_set_color(0xFF);
        debug_textdraw_vsprintf_buf_1("X+A: Normal Mode");
    } else {
        debug_textdraw_set_pos(0x22, 0x24);
        debug_textdraw_vsprintf_buf_1("X+A: All Disp Mode");
    }
    if (b1AE >= 0) {
        for (i = 0; (u32) i < 8; i++) {
            debug_textdraw_set_pos(1, (((i - s_sel[0]) + 8) & 7) + 3);
            if (i == s_sel[0]) {
                if (b1AE == 0) {
                    debug_textdraw_set_color(0x33);
                } else {
                    debug_textdraw_set_color(0xFF);
                }
                debug_textdraw_vsprintf_buf_1("%-8s\034", D22C4[i]);
            } else if (b1AE == 0) {
                debug_textdraw_vsprintf_buf_1("%-8s", D22C4[i]);
            }
            debug_textdraw_set_color(0xFF);
        }
    }
    if (b1AE >= 1) {
        if (model_count(s_sel[0]) < 0x14) {
            for (i = 0; i < model_count(s_sel[0]); i++) {
                debug_textdraw_set_pos(0xA, (((i - s_sel[1]) + model_count(s_sel[0])) % model_count(s_sel[0])) + 3);
                if (i == s_sel[1]) {
                    if (b1AE == 1) {
                        debug_textdraw_set_color(0x33);
                    } else {
                        debug_textdraw_set_color(0xFF);
                    }
                    debug_textdraw_vsprintf_buf_1("%-8s\034", model_name(s_sel[0], i + 1));
                } else if (b1AE == 1) {
                    debug_textdraw_vsprintf_buf_1("%-8s", model_name(s_sel[0], i + 1));
                }
                debug_textdraw_set_color(0xFF);
            }
        } else {
            for (i = 0; i < model_count(s_sel[0]); i++) {
                if ((((i - s_sel[1]) + model_count(s_sel[0])) % model_count(s_sel[0])) < 0x14) {
                    debug_textdraw_set_pos(0xA, (((i - s_sel[1]) + model_count(s_sel[0])) % model_count(s_sel[0])) + 3);
                    if (i == s_sel[1]) {
                        debug_textdraw_set_color(0x33);
                        debug_textdraw_vsprintf_buf_1("%-8s\034", model_name(s_sel[0], i + 1));
                    } else if (b1AE == 1) {
                        debug_textdraw_vsprintf_buf_1("%-8s", model_name(s_sel[0], i + 1));
                        debug_textdraw_set_pos(0xD, 2);
                        debug_textdraw_vsprintf_buf_1("\036");
                        debug_textdraw_set_pos(0xD, 0x17);
                        debug_textdraw_vsprintf_buf_1("\037");
                    }
                    debug_textdraw_set_color(0xFF);
                }
            }
        }
    }
    if (b1AE >= 2) {
        debug_textdraw_set_pos(0x13, 3);
        if ((void *) b198 == NULL) {
            debug_textdraw_vsprintf_buf_1(" no data.");
            return;
        }
        model = b198->modelEntries[b1A8].model;
        debug_textdraw_vsprintf_buf_1(" [%d/%d] %-8s\n", b1A8, b198->numModels, b198->modelEntries[b1A8].name);
        if (model != NULL) {
            if ((u32) (model->flags & 4) != 0) {
                debug_textdraw_vsprintf_buf_1("         [ Stitching Model ]\n");
                debug_textdraw_vsprintf_buf_1("           Matrix Num:%d\n", model->mtxCount);
            }
            if ((u32) (model->flags & 8) != 0) {
                debug_textdraw_vsprintf_buf_1("         [ Skin Model ]\n");
                debug_textdraw_vsprintf_buf_1("           Matrix Num:%d\n", model->mtxCount);
            }
            if ((u32) (model->flags & 0x10) != 0) {
                debug_textdraw_vsprintf_buf_1("         [ Effective Model ]\n");
                debug_textdraw_vsprintf_buf_1("           Matrix Num:%d\n", model->mtxCount);
            }
        }
    }
}

void model_test_unused2(void)
{
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

    GXSetChanMatColor(0, color);
    debug_textdraw_vsprintf_buf_1("rad: %-3.6f     ");
    mtxa_scale_s(90.0f);
    mtxa_scale_s(250.0f);
    mtxa_scale_s(0.999f);
}

static inline void mtxa_point_on_z(register float z, register Vec *out)
{
    register float *mtxA;
    register float x1, x2, y1, y2, z1, z2;

    asm
    {
        lis mtxA, LC_CACHE_BASE@ha
        lfs x1, 0x08(mtxA)
        lfs x2, 0x0C(mtxA)
        lfs y1, 0x18(mtxA)
        lfs y2, 0x1C(mtxA)
        lfs z1, 0x28(mtxA)
        lfs z2, 0x2C(mtxA)
        fmadds x1, x1, z, x2
        stfs x1, out->x
        fmadds y1, y1, z, y2
        stfs y1, out->y
        fmadds z1, z1, z, z2
        stfs z1, out->z
    }
}

void fn_8_89B4(void)
{
    GXLightObj light;
    Vec pos1;
    Vec pos2;
    Vec pos3;
    Vec viewPos;
    GXColor mat = {0xFF, 0xFF, 0xFF, 0x32};
    GXColor amb = {0x64, 0x64, 0x64, 0x32};
    GXColor red = {0xFF, 0x00, 0x00, 0xFF};
    GXColor green = {0x00, 0xFF, 0x00, 0xFF};
    GXColor blue = {0x00, 0x00, 0xFF, 0xFF};

    mtxa_from_translate(&g_current_camera->lookAt);
    mtxa_rotate_y((s16) (int) (1.2f * (u32)(frame_counter << 8)));
    mtxa_point_on_z(10.0f, &pos1);
    mtxa_from_translate(&g_current_camera->lookAt);
    mtxa_rotate_x((s16) (frame_counter << 8));
    mtxa_point_on_z(-10.0f, &pos2);
    mtxa_from_translate(&g_current_camera->lookAt);
    mtxa_rotate_z((s16) (int) (0.8f * (u32)(frame_counter << 8)));
    mtxa_tf_point_xyz(0.0f, 10.0f, 0.0f, &pos3);
    GXInitLightSpot(&light, 0.0f, 0);
    GXInitLightDistAttn(&light, 150.0f, 0.2f, 3);
    mtxa_tf_point(&pos1, &viewPos);
    GXInitLightPos(&light, viewPos.x, viewPos.y, viewPos.z);
    GXInitLightColor(&light, green);
    GXLoadLightObjImm(&light, 4);
    GXInitLightSpot(&light, 0.0f, 0);
    GXInitLightDistAttn(&light, 150.0f, 0.2f, 3);
    mtxa_tf_point(&pos2, &viewPos);
    GXInitLightPos(&light, viewPos.x, viewPos.y, viewPos.z);
    GXInitLightColor(&light, blue);
    GXLoadLightObjImm(&light, 8);
    GXInitLightSpot(&light, 0.0f, 0);
    GXInitLightDistAttn(&light, 150.0f, 0.2f, 3);
    mtxa_tf_point(&pos3, &viewPos);
    GXInitLightPos(&light, viewPos.x, viewPos.y, viewPos.z);
    GXInitLightColor(&light, red);
    GXLoadLightObjImm(&light, 0x10);
    GXSetChanMatColor(0, mat);
    GXSetChanAmbColor(0, amb);
    opti_GXSetChanCtrl(4, 1, 0, 0, 0x1C, 2, 2);
    GXSetNumChans_cached(1);
    g_GXSetTevOrder_wrapper(0, 0xFF, 0xFF, 4);
    g_GXSetTevColorIn_GXSetTevAlphaIn_wrapper(0, 4);
    GXSetNumTexGens_cached(0);
    GXSetNumTevStages_cached(1);
    mtxa_from_mtxb_translate(&g_current_camera->lookAt);
    mtxa_scale_s(0.05f);
    GXLoadNrmMtxImm(mtxa->mtxA, 0);
    GXLoadPosMtxImm(mtxa->mtxA, 0);
    GXSetCurrentMtx(0);
    GXDrawSphere(8, 8);
}

static inline void model_free(void)
{
    if (b194 != NULL || b198 != NULL)
    {
        gp_wait_wrapper();
    }
    if (b194 != NULL)
    {
        g_something_freeing_heap_2(b194);
        b194 = NULL;
    }
    if (b198 != NULL)
    {
        g_something_freeing_heap(b198);
        b198 = NULL;
    }
}

void fn_8_8D74(void)
{
    u32 i;

    switch (sub_mode) {
    case 0x8F:
    case 0x90:
        set_sub_mode_destination(fn_8_8D74);
        return;
    }
    model_free();
    for (i = 0; i < (u32) b5C0; i++) {
        OSFreeToHeap(currentHeap, b1C0[i]);
    }
}

static inline void model_load(void)
{
    char path[0x80];
    struct ModelEntry *entry = &D3208[b1A4];

    sprintf(path, "%s/%s.tpl", entry->dir, entry->name);
    b194 = g_load_tpl(path);
    sprintf(path, "%s/%s.gma", entry->dir, entry->name);
    b198 = g_load_gma(path, b194);
    if (b198 == NULL && b194 != NULL)
    {
        g_something_freeing_heap_2(b194);
        b194 = NULL;
    }
}

void fn_8_8E60(void)
{
    int i;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        b1A8 = 0;
        b1A0 = 0;
        b1A4 = 0;
        b1AC = 0;
        model_load();
        event_init(0x10);
        enable_cameras_for_active_players();
        camera_set_state(2);
        g_current_camera->eye.x = 0.0f;
        g_current_camera->eye.y = 5.0f;
        g_current_camera->eye.z = 20.0f;
        g_current_camera->lookAt.x = 0.0f;
        g_current_camera->lookAt.y = 0.0f;
        g_current_camera->lookAt.z = 0.0f;
        for (i = 0; i < 3; i++) {
            s_sel[i] = 0;
            b5C4[i] = 0;
        }
        b1AE = 0;
        b5CA = 1;
        b5CB = 1;
        b1B8 = 0;
        b1BC = 0.0f;
        set_sub_mode_destination(fn_8_8D74);
        sub_mode_request = 0x90;
    }
}

static inline int model_find(int type, int idx)
{
    u32 i;
    int n = 0;

    for (i = 0; i < 0x22A; i++)
    {
        if (type == D3208[i].type)
            n++;
        if (idx + 1 == n)
            return i;
    }
    return -1;
}

static inline void model_view_update(void)
{
    struct GMAModel *model = b198->modelEntries[b1A8].model;

    if (b1B8 == 0 || 0.0 == b1BC)
        b1BC = model->boundSphereRadius;
    if (b1B8 != 0)
    {
        g_current_camera->lookAt = model->boundSphereCenter;
        g_current_camera->eye = g_current_camera->lookAt;
        g_current_camera->eye.y += 0.5f * b1BC;
        g_current_camera->eye.z += 1.0f + (2.0f * b1BC);
    }
    else
    {
        g_current_camera->lookAt = model->boundSphereCenter;
        g_current_camera->eye = g_current_camera->lookAt;
        g_current_camera->eye.y += 0.5f * model->boundSphereRadius;
        g_current_camera->eye.z += 1.0f + (2.0f * model->boundSphereRadius);
    }
}

void fn_8_9074(void)
{
    float tf2;
    float tf3;
    int v28;
    int v27;

    v28 = 0;
    v27 = 0;
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        fn_8_7DDC();
        if (PAD[2].button & 0x800) {
            b5CA = (u8) (b5CA ^ 1);
            b5CB = (u8) (b5CB ^ 1);
        }
        if (PAD[2].button & 0x10) {
            b1B8 = (u8) (b1B8 ^ 1);
        }
        if (((PAD[0].button & 0x400) && (PAD[2].button & 0x100)) || ((PAD[0].button & 0x100) && (PAD[2].button & 0x400))) {
            b1A0 = (b1A0 ^ 1);
        } else if (PAD[2].button & 0x100) {
            if (b1AE < 2) {
                b1AE++;
            }
        } else if (PAD[2].button & 0x200) {
            if (b1AE > 0) {
                s_sel[b1AE] = 0;
                b1AE--;
            }
        } else if (PAD[2].button & 0x400) {
            b1B6 = (s16) (b1B6 ^ 1);
            if (b1B6 != 0) {
                camera_set_state(3);
            } else {
                camera_set_state(2);
            }
        }
        if (b1B6 != 0) {
            tf2 = PAD[0].stickX;
            tf3 = PAD[0].stickY;
            if (fabs(tf2) > 4.0) {
                b5CC[0] += -0.0027777778f * tf2;
            }
            if (fabs(tf3) > 4.0) {
                b5CC[1] += 0.0027777778f * tf3;
            }
            if (PAD[0].button & 0x100) {
                b5D8 = 0xFFF;
            }
            if (b5DC < 1.0f) {
                b5DC += 0.13333333333333333;
            }
            if (b5D8 < 0xFFF) {
                b5D8 += 2;
            }
        }
        if ((b1AE == 2) && (PAD[2].button & 0x100)) {
            if (b1A4 != model_find(s_sel[0], s_sel[1])) {
                b1A4 = model_find(s_sel[0], s_sel[1]);
                v28 = 1;
            }
        }
        if ((b1A0 == 0) && (b198 != NULL)) {
            if (((PAD[2].button & 1) || (PAD[4].button & 1) || ((PAD[0].button & 1) && (PAD[0].button & 0x20))) && (b1AE >= 2)) {
                b1A8--;
                while (1) {
                    if (b1A8 < 0) {
                        b1A8 = b198->numModels - 1;
                    }
                    if (b198->modelEntries[b1A8].model != NULL) {
                        break;
                    }
                    b1A8--;
                }
                model_view_update();
                v27 = 1;
            } else if (((PAD[2].button & 2) || (PAD[4].button & 2) || ((PAD[0].button & 2) && (PAD[0].button & 0x20))) && (b1AE >= 2)) {
                b1A8++;
                while (1) {
                    if (b1A8 >= (int) b198->numModels) {
                        b1A8 = 0;
                    }
                    if (b198->modelEntries[b1A8].model != NULL) {
                        break;
                    }
                    b1A8++;
                }
                model_view_update();
                v27 = 1;
            }
        }
        if (v28 != 0) {
            model_free();
            if (b1AC != b1A4) {
                model_load();
                b1AC = b1A4;
            }
            b1A8 = 0;
            if (b198 != NULL) {
                while (1) {
                    if (b1A8 >= (int) b198->numModels) {
                        b1A8 = 0;
                    }
                    if (b198->modelEntries[b1A8].model != NULL) {
                        break;
                    }
                    b1A8++;
                }
                model_view_update();
            }
            b1A8 = 0;
            v27 = 1;
        }
        if (v27 != 0) {
            b5CC[0] = 0.0f;
            b5CC[1] = 0.0f;
            b5E0 = -1;
            b5DC = 0.0f;
            b5D8 = 0;
        }
    }
}

void fn_8_9C44(void)
{
    GXLightObj light;
    Vec pos;
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    int i;
    struct GMAModel *m;
    struct GMAModel *model;
    char *unused = D22C4[0];

    if (b198 != NULL) {
        if (b5CB != 0) {
            m = b198->modelEntries[b1A8].model;
            mtxa_from_mtxb();
            if (m != NULL) {
                mtxa_scale_s((0.5 * m->boundSphereRadius));
            }
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[90].model);
        }
        mtxa_from_mtxb();
        mtxa_tf_point_xyz(0.0f, 150.0f, 90.0f, &pos);
        GXInitLightSpot(&light, 0.0f, 0);
        GXInitLightDistAttn(&light, 250.0f, 0.999f, 0);
        GXInitLightPos(&light, pos.x, pos.y, pos.z);
            GXInitLightColor(&light, color);
        GXLoadLightObjImm(&light, 1);
        mtxa_from_mtxb();
        GXLoadPosMtxImm(mtxa->mtxA, 0);
        GXLoadNrmMtxImm(mtxa->mtxA, 0);
        if (b5CA != 0) {
            fn_8_89B4();
        }
        debug_textdraw_set_pos(1, 0x23);
        if ((b1AC == b1A4) && (b198 != NULL)) {
            mtxa_from_mtxb();
            GXLoadPosMtxImm(mtxa->mtxA, 0);
            GXLoadNrmMtxImm(mtxa->mtxA, 0);
            perf_init_timer(0);
            if (b1A0 != 0) {
                for (i = 0; i < (int) b198->numModels; i++) {
                    model = b198->modelEntries[i].model;
                    if (model != NULL) {
                        if ((u32) (model->flags & 0xC) != 0) {
                            fn_8_9FA4(model);
                        }
                        avdisp_draw_model_culled_sort_never(model);
                    }
                }
            } else {
                model = b198->modelEntries[b1A8].model;
                if (model != NULL) {
                    if ((u32) (model->flags & 0xC) != 0) {
                        fn_8_9FA4(model);
                    }
                    avdisp_draw_model_culled_sort_never(model);
                    debug_textdraw_set_pos(0x14, 1);
                    debug_textdraw_vsprintf_buf_1("rad: %-3.6f     ", model->boundSphereRadius);
                    if (b1B6 != 0) {
                        fn_8_A0E4(model);
                    }
                }
            }
            debug_textdraw_set_pos(0x28, 1);
            debug_textdraw_vsprintf_buf_1("Time: %d\n", perf_stop_timer(0));
            if (b5CB != 0) {
                model = b198->modelEntries[b1A8].model;
                mtxa_from_mtxb();
                avdisp_set_alpha(0.2f);
                if (model != 0) {
                    mtxa_translate(&model->boundSphereCenter);
                }
                mtxa_scale_s((b1BC / 0.505));
                GXLoadPosMtxImm(mtxa->mtxA, 0);
                GXLoadNrmMtxImm(mtxa->mtxA, 0);
                avdisp_draw_model_unculled_sort_never(init_common_gma->modelEntries[40].model);
            }
            b190 += 0x200;
        }
    }
}

void fn_8_9FA4(struct GMAModel *model)
{
    float rad;
    int count;
    Mtx *mtxs;
    Mtx *m;
    s16 angle;
    s16 step;
    int n;
    Mtx **list;

    if ((PAD[0].button & 0x100) == 0) {
        draw_poly(model, 0);
        return;
    }
    n = model->mtxCount;
    list = g_some_buffer_ptr;
    mtxs = (Mtx *) (list + n);
    {
        Mtx **p = list;

        m = mtxs;
        count = n;
        while (count > 0) {
            *p = m;
            count--;
            p++;
            m++;
        }
    }
    draw_poly(model, list);
    mtxa_push();
    rad = model->boundSphereRadius;
    angle = g_font_char_var_3 << 6;
    step = 0x10000 / n;
    m = mtxs;
    count = n;
    while (count > 0) {
        mtxa_from_translate_xyz(0.0f, 0.0f, rad * math_sin(angle));
        mtxa_mult_left(*m);
        mtxa_to_mtx(*m);
        count--;
        m++;
        angle += step;
    }
    mtxa_pop();
    pointer_range_advance(model, mtxs);
}

static char *D4DF8[15] = {
    "I4", "I8", "IA4", "IA8", "RGB565",
    "RGB5A3", "RGBA8", "?", "?", "?",
    "?", "?", "?", "?", "CMPR",
};

static char *D4E4C[3] = {
    "CLAMP", "REPEAT", "MIRROR",
};

static char *D4EC4[6] = {
    "NEAR", "LINEAR", "NEAR & MIPMAP NEAR",
    "LINEAR & MIPMAP NEAR", "NEAR & MIPMAP LINEAR", "LINEAR & MIPMAP LINEAR",
};

static char *D4EF4[3] = {
    "ANISO 1", "ANISO 2", "ANISO 4",
};

void fn_8_A0E4(struct GMAModel *model)
{
    char buf[0x100];
    struct ShadowDebugQuad quad;
    struct ShadowDebugQuad quad2;
    GXTexObj texObj;
    u8 *image;
    int format;
    int wrapS;
    int wrapT;
    int minFilter;
    int magFilter;
    float minLod;
    float maxLod;
    float lodBias;
    int aniso;
    u16 width;
    u16 height;
    u8 mipmap;
    u8 biasClamp;
    u8 edgeLod;
    float invW;
    float invH;
    float w;
    float h;
    float qw;
    float qh;
    float step;
    int i;
    int selected;
    int lod;
    int texCount;
    GXTexObj *tex;
    int found;
    u32 len;
    int count;

    memset(&quad, 0, sizeof(quad));
    w = 640.0f * g_current_camera->sub28.viewportW;
    h = 480.0f * g_current_camera->sub28.viewportH;
    invW = 1.0f / w;
    invH = 1.0f / h;
    textdraw_reset();
    textdraw_set_font(1);
    step = 0.06666667f;
    quad.x = -0.9f + b5CC[0];
    quad.y = -0.9f + b5CC[1];
    quad.z = -0.1f;
    if ((PAD[0].button & 0x100) == 0) {
        quad.flag = 0;
    } else {
        quad.flag = 1;
    }
    quad.color[0] = 0xFF;
    quad.color[1] = 0xFF;
    quad.color[2] = 0xFF;
    quad.color[3] = 0xFF;
    found = 0;
    tex = model->texObjs;
    texCount = model->tevLayerCount;
    i = 0;
    while (i < texCount) {
        quad.y += 0.5f * step;
        GXGetTexObjAll(tex, &image, &width, &height, &format, &wrapS, &wrapT, &mipmap);
        g_GX_something(tex, &minFilter, &magFilter, &minLod, &maxLod, &lodBias, &biasClamp, &edgeLod, &aniso);
        qw = 2.0f * (width * invW);
        qh = 2.0f * (height * invH);
        if ((found == 0) && (((quad.y + qh) > 0.0f) || (i == (texCount - 1)))) {
            if (b5E0 != i) {
                b5DC = 0.0f;
                b5D8 = 0;
                b5E0 = (s16) i;
            }
            found = 1;
        }
        if (b5E0 == i) {
            selected = 1;
        } else {
            selected = 0;
        }
        if ((quad.y > -(1.0f + step)) && (quad.y < 1.0f)) {
            if (selected == 0) {
                textdraw_set_pos(640.0f * (0.5f * (1.0f + quad.x)), 480.0f * (0.5f * (1.0f + quad.y)));
                textdraw_printf("[%d/%d] ( %d x %d ) Format:%s", i + 1, texCount, width, height, D4DF8[format]);
            } else {
                textdraw_set_mul_color(0xFF80FF80);
                textdraw_set_pos(640.0f * (0.5f * (1.0f + (quad.x - (0.05f * b5DC)))), 480.0f * (0.5f * (1.0f + quad.y)));
                textdraw_printf("[%d/%d] ( %d x %d ) Format:%s ", i + 1, texCount, width, height, D4DF8[format]);
                if (b5D8 > 0) {
                    sprintf(buf, "Wrap( %s, %s ) ", D4E4C[wrapS], D4E4C[wrapT]);
                    if (mipmap != 0) {
                        sprintf(&buf[strlen(buf)], "Mipmap ");
                    }
                    sprintf(&buf[strlen(buf)], "[%s, %s] %s", D4EC4[minFilter], D4EC4[magFilter], D4EF4[aniso]);
                    if (strlen(buf) < (u32) b5D8) {
                        len = strlen(buf);
                    } else {
                        len = (u32) b5D8;
                    }
                    textdraw_printf("%s\n", &buf[strlen(buf)] - len);
                }
                textdraw_set_mul_color(-1);
            }
        }
        quad.y += 1.2f * step;
        if (mipmap != 0) {
            count = (u32) maxLod + 1;
        } else {
            count = 1;
        }
        quad2 = quad;
        for (lod = 0; lod < count; lod++) {
            GXInitTexObj(&texObj, image, width, height, format, wrapS, wrapT, 0);
            GXInitTexObjLOD(&texObj, minFilter, magFilter, 0.0f, maxLod - lod, lodBias, biasClamp, edgeLod, aniso);
            quad2.x2 = quad2.x + qw;
            quad2.y2 = quad2.y + qh;
            fn_1_81E4(&texObj, &quad2);
            quad2.x = quad2.x2;
            image += GXGetTexBufferSize(width, height, format, 0, 0);
            width >>= 1;
            height >>= 1;
        }
        quad2.color[0] = 0xFF;
        quad2.color[1] = 0;
        quad2.color[2] = 0;
        quad2.x += 0.1f;
        while ((width != 0) && (height != 0)) {
            GXInitTexObj(&texObj, image, width, height, format, wrapS, wrapT, 0);
            GXInitTexObjLOD(&texObj, minFilter, magFilter, 0.0f, 0.0f, lodBias, biasClamp, edgeLod, aniso);
            quad2.x2 = quad2.x + (0.5f * qw);
            quad2.y2 = quad2.y + (0.5f * qh);
            fn_1_81E4(&texObj, &quad2);
            quad2.x = quad2.x2;
            image += GXGetTexBufferSize(width, height, format, 0, 0);
            width >>= 1;
            height >>= 1;
        }
        quad.y += qh;
        quad.z += -0.1f;
        i++;
        tex++;
    }
}

static char *D5090[5][4] = {
    "MONKEY BALL", "\203\202\203\223\203L\201[", "pattern.gma", "pattern.tpl",
    "SPIKE OUT", "\203X\203p\203C\203N\203A\203E\203g", "pattern_spk.gma", "pattern_spk.tpl",
    "HARRIER", "\203v\203\211\203l\203b\203g\203n\203\212\203A\201[", "pattern_har.gma", "pattern_har.tpl",
    "SLASH OUT", "\203X\203\211\203b\203V\203\205\203A\203E\203g", "pattern_gem.gma", "pattern_gem.tpl",
    "SPIKERS", "\203X\203p\203C\203J\201[\203Y\203o\203g\203\213", "pattern_spkb.gma", "pattern_spkb.tpl",
};

void fn_8_A82C(int arg0)
{
    u8 *name;
    u32 len;
    int i;
    int count;
    char **entry;

    entry = D5090[arg0];
    DVDChangeDir("init");
    b5E4->tpl = g_load_tpl(entry[3]);
    if (b5E4->tpl != NULL) {
        b5E4->gma = g_load_gma(entry[2], b5E4->tpl);
    }
    DVDChangeDir("..");
    if ((int) b5E4->gma->numModels > 0) {
        count = 0;
        for (i = 0; i < (int) b5E4->gma->numModels; count++) {
            name = (u8 *) b5E4->gma->modelEntries[i].name;
            for (len = strlen((char *) name); name[len - 1] >= '0' && name[len - 1] <= '9'; len--) {
            }
            for (i++; i < (int) b5E4->gma->numModels; i++) {
                if (strncmp((char *) name, b5E4->gma->modelEntries[i].name, len) != 0) {
                    break;
                }
            }
        }
        b5E4->starts = alloc_from_heap_or_panic(currentHeap, count * 4, "test_mode.c", 0x18E1);
        b5E4->ends = alloc_from_heap_or_panic(currentHeap, count * 4, "test_mode.c", 0x18E2);
        b5E4->names = alloc_from_heap_or_panic(currentHeap, count * sizeof(char *), "test_mode.c", 0x18E3);
        b5E4->groupCount = count;
        count = 0;
        for (i = 0; i < (int) b5E4->gma->numModels; count++) {
            name = (u8 *) b5E4->gma->modelEntries[i].name;
            for (len = strlen((char *) name); name[len - 1] >= '0' && name[len - 1] <= '9'; len--) {
            }
            b5E4->starts[count] = i;
            b5E4->names[count] = alloc_from_heap_or_panic(currentHeap, len + 1, "test_mode.c", 0x18EE);
            strncpy(b5E4->names[count], (char *) name, len);
            b5E4->names[count][len] = 0;
            for (i++; i < (int) b5E4->gma->numModels; i++) {
                if (strncmp((char *) name, b5E4->gma->modelEntries[i].name, len) != 0) {
                    break;
                }
            }
            b5E4->ends[count] = i;
        }
    }
}

void fn_8_ABE4(void)
{
    event_init(0x10);
    enable_cameras_for_active_players();
    camera_set_state(2);
    g_current_camera->eye.x = 0.0f;
    g_current_camera->eye.y = 0.3f;
    g_current_camera->eye.z = 2.0f;
    g_current_camera->lookAt.x = 0.0f;
    g_current_camera->lookAt.y = 0.0f;
    g_current_camera->lookAt.z = 0.0f;
    b5E4 = alloc_from_heap_or_panic(currentHeap, sizeof(struct PatTest), "test_mode.c", 0x1931);
    memset(b5E4, 0, sizeof(struct PatTest));
    set_sub_mode_destination(fn_8_B14C);
    sub_mode_request = 0x92;
}

static inline void pat_free_model(void) {
    int v31;

    if ((b5E4->tpl != NULL) || (b5E4->gma != NULL)) {
        VISetNextFrameBuffer(*graphics_info);
        VIWaitForRetrace();
    }
    if (b5E4->tpl != NULL) {
        g_something_freeing_heap_2(b5E4->tpl);
        b5E4->tpl = NULL;
    }
    if (b5E4->gma != NULL) {
        g_something_freeing_heap(b5E4->gma);
        b5E4->gma = NULL;
    }
    if (b5E4->names != NULL) {
        for (v31 = 0; v31 < b5E4->groupCount; v31++) {
            OSFreeToHeap(currentHeap, b5E4->names[v31]);
        }
        OSFreeToHeap(currentHeap, b5E4->names);
        b5E4->names = NULL;
    }
    if (b5E4->starts != NULL) {
        OSFreeToHeap(currentHeap, b5E4->starts);
        b5E4->starts = NULL;
    }
    if (b5E4->ends != NULL) {
        OSFreeToHeap(currentHeap, b5E4->ends);
        b5E4->ends = NULL;
    }
}

void fn_8_ACFC(void)
{
    int v4;
    int v5;
    int v6;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        switch (b5E4->state) {
        case 0:
            v4 = b5E4->category;
            if (PAD[4].button & 4) {
                v4++;
                if ((u32) v4 >= 5) {
                    v4 = 0;
                }
            }
            if (PAD[4].button & 8) {
                v4--;
                if (v4 < 0) {
                    v4 = 4;
                }
            }
            b5E4->category = v4;
            if (PAD[2].button & 0x100) {
                fn_8_A82C(b5E4->category);
                b5E4->group = 0;
                b5E4->state = 1;
                return;
            }
            break;
        case 1:
            v5 = b5E4->group;
            if (PAD[4].button & 4) {
                v5++;
                if (v5 >= b5E4->groupCount) {
                    v5 = 0;
                }
            }
            if (PAD[4].button & 8) {
                v5--;
                if (v5 < 0) {
                    v5 = b5E4->groupCount - 1;
                }
            }
            b5E4->group = v5;
            v6 = b5E4->speed;
            if (PAD[4].button & 2) {
                v6++;
                if (v6 > 0xA) {
                    v6 = 0xA;
                }
            }
            if (PAD[4].button & 1) {
                v6--;
                if (v6 < 0) {
                    v6 = 0;
                }
            }
            b5E4->speed = v6;
            if (PAD[2].button & 0x200) {
                pat_free_model();
                b5E4->state = 0;
            }
            break;
        }
    }
}

void fn_8_B14C(void)
{
    switch (sub_mode) {
    case 0x91:
    case 0x92:
        set_sub_mode_destination(fn_8_B14C);
        return;
    }
    pat_free_model();
    OSFreeToHeap(currentHeap, b5E4);
    b5E4 = NULL;
}

void fn_8_B3C4(void)
{
    int tr5;
    char **v28;
    int x;
    int v26;
    int tr6;
    int v29;
    int tr26;

    debug_textdraw_set_pos(1, 1);
    if (g_is_sjis_and_more_than_24_mib() != 0) {
        debug_textdraw_vsprintf_buf_1("\203p\203^\201[\203\223\203`\203F\203\223\203W");
    } else {
        debug_textdraw_vsprintf_buf_1("PATTERN CHANGE");
    }
    switch (b5E4->state) {
    case 0:
        x = 4;
        v26 = 4;
        v28 = D5090[0];
        v29 = 0;
        while ((u32) v29 < 5) {
            if (v29 == b5E4->category) {
                if (g_is_sjis_and_more_than_24_mib() != 0) {
                    debug_textdraw_set_pos(x - 3, v26);
                    debug_textdraw_vsprintf_buf_1("\201\237");
                } else {
                    debug_textdraw_set_pos(x - 2, v26);
                    debug_textdraw_vsprintf_buf_1("*");
                }
            }
            debug_textdraw_set_pos(x, v26);
            if (g_is_sjis_and_more_than_24_mib() != 0) {
                debug_textdraw_vsprintf_buf_1(v28[1]);
            } else {
                debug_textdraw_vsprintf_buf_1(v28[0]);
            }
            v29++;
            v26 += 2;
            v28 += 4;
        }
        textdraw_set_font(3);
        textdraw_set_drop_shadow();
        textdraw_set_scale(1.0f, 1.0f);
        textdraw_set_drop_shadow_with_params(2, 2, 1.0f);
        textdraw_set_font_style(0);
        textdraw_set_mul_color(-1);
        textdraw_set_alpha(1.0f);
        textdraw_set_depth(0.11f);
        textdraw_set_alignment(8);
        textdraw_set_pos(630.000000f, 470.000000f);
        textdraw_print("/cd839f/\214\210\222\350");
        return;
    case 1:
        debug_textdraw_set_pos(4, 4);
        debug_textdraw_put_buf_1("speed: ");
        debug_textdraw_set_color(0xC3);
        debug_textdraw_vsprintf_buf_1("%s%d%s", b5E4->speed == 0 ? " " : "\035", b5E4->speed, b5E4->speed < 10 ? "\034" : " ");
        debug_textdraw_set_color(0xFF);
        debug_textdraw_set_pos(4, 6);
        for (v29 = 0; v29 < b5E4->groupCount; v29++) {
            if (v29 == b5E4->group) {
                debug_textdraw_set_color(0xC3);
                fn_1_3E100(-1, 0);
                debug_textdraw_put_buf_1("\034");
                debug_textdraw_set_color(0xFF);
            }
            debug_textdraw_vsprintf_buf_1("[%2d] %s\n", b5E4->ends[v29] - b5E4->starts[v29], b5E4->names[v29]);
        }
        textdraw_set_font(3);
        textdraw_set_drop_shadow();
        textdraw_set_scale(1.0f, 1.0f);
        textdraw_set_drop_shadow_with_params(2, 2, 1.0f);
        textdraw_set_font_style(0);
        textdraw_set_mul_color(-1);
        textdraw_set_alpha(1.0f);
        textdraw_set_depth(0.11f);
        textdraw_set_alignment(8);
        textdraw_set_pos(630.000000f, 470.000000f);
        textdraw_print("/cd83a0/\202\340\202\307\202\351");
        tr5 = b5E4->starts[b5E4->group];
        tr6 = b5E4->ends[b5E4->group];
        tr26 = tr5 + (((u32) g_font_char_var_3 / (u32) (b5E4->speed + 1)) % (tr6 - tr5));
        debug_textdraw_set_pos(1, 0x23);
        debug_textdraw_put_buf_1(b5E4->gma->modelEntries[tr26].name);
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_culled_sort_auto(b5E4->gma->modelEntries[tr26].model);
        break;
    }
}

void fn_8_B818(void)
{
    if ((u32) lbl_1_bss_D2B7C != 0) {
        fn_1_F0E4C(0x18, 0);
    }
    sub_mode_request = 0x94;
}

/* Unreferenced in the shipped REL; only its string literals survive. */
void romfont_test_unused(void) {
    debug_textdraw_vsprintf_buf_1("ROMFONT TEST\n");
    debug_textdraw_vsprintf_buf_1("code: %04X\n", b5E8);
}

void fn_8_B864(void)
{
    int v3;

    v3 = b5E8;
    if (PAD_REPEAT(4)) {
        v3++;
        if ((u32) v3 >= 0x18) {
            v3 = 0;
        }
    }
    if (PAD_REPEAT(8)) {
        v3--;
        if (v3 < 0) {
            v3 = 0x17;
        }
    }
    b5E8 = v3;
}

static struct RomFont D5150[100] = {
    { 201, 4096, 1.0f },
    { 202, 4096, 1.0f },
    { 203, 4096, 1.0f },
    { 204, 4096, 1.0f },
    { 1, 4096, 1.0f },
    { 2, 4096, 1.0f },
    { 3, 4096, 1.0f },
    { 4, 6144, 1.0f },
    { 5, 4096, 1.0f },
    { 6, 4096, 1.0f },
    { 7, 6144, 1.0f },
    { 8, 6144, 1.0f },
    { 9, 6144, 1.0f },
    { 10, 6144, 1.0f },
    { 11, 6144, 1.0f },
    { 12, 6144, 1.0f },
    { 13, 6144, 1.0f },
    { 14, 6144, 1.0f },
    { 15, 6144, 1.0f },
    { 16, 6144, 1.0f },
    { 231, 4096, 1.0f },
    { 232, 6144, 1.0f },
    { 233, 6144, 1.0f },
    { 234, 6144, 1.0f },
    { 235, 4096, 1.0f },
    { 236, 6144, 1.0f },
    { 237, 4096, 1.0f },
    { 238, 4096, 1.0f },
    { 239, 4096, 1.0f },
    { 17, 6144, 1.0f },
    { 18, 4096, 1.0f },
    { 19, 6144, 1.0f },
    { 20, 6144, 1.0f },
    { 21, 4096, 1.0f },
    { 22, 4096, 1.0f },
    { 23, 6144, 1.0f },
    { 24, 6144, 1.0f },
    { 25, 6144, 1.0f },
    { 26, 6144, 1.0f },
    { 27, 6144, 1.0f },
    { 28, 6144, 1.0f },
    { 29, 6144, 1.0f },
    { 30, 4096, 1.0f },
    { 31, 6144, 1.0f },
    { 32, 4096, 1.0f },
    { 33, 10240, 1.0f },
    { 34, 6144, 1.0f },
    { 35, 6144, 1.0f },
    { 36, 4096, 1.0f },
    { 37, 4096, 1.0f },
    { 38, 0, 1.0f },
    { 39, 6144, 1.0f },
    { 40, 6144, 1.0f },
    { 41, 6144, 1.0f },
    { 42, 6144, 1.0f },
    { 43, 2048, 1.0f },
    { 44, 4096, 1.0f },
    { 45, 6144, 1.0f },
    { 46, 6144, 1.0f },
    { 47, 6144, 1.0f },
    { 281, 4096, 1.0f },
    { 282, 6144, 1.0f },
    { 283, 4096, 1.0f },
    { 284, 6144, 1.0f },
    { 285, 6144, 1.0f },
    { 286, 4096, 1.0f },
    { 287, 4096, 1.0f },
    { 288, 6144, 1.0f },
    { 289, 6144, 1.0f },
    { 48, 4096, 1.0f },
    { 49, 5120, 1.0f },
    { 50, 5120, 1.0f },
    { 51, 6144, 1.0f },
    { 52, -24576, 0.3f },
    { 53, 5120, 1.0f },
    { 54, 6144, 1.0f },
    { 55, 6144, 1.0f },
    { 56, 4608, 1.0f },
    { 57, 6144, 1.0f },
    { 58, 5120, 1.0f },
    { 59, 6144, 1.0f },
    { 60, 6144, 1.0f },
    { 61, 4096, 1.0f },
    { 62, 4096, 1.0f },
    { 63, 4096, 1.0f },
    { 64, 6144, 1.0f },
    { 65, 6144, 1.0f },
    { 66, 4096, 1.0f },
    { 67, 5120, 1.0f },
    { 68, 6144, 1.0f },
    { 341, 6144, 1.0f },
    { 342, 6144, 1.0f },
    { 343, 6144, 1.0f },
    { 344, 6144, 1.0f },
    { 345, 5120, 1.0f },
    { 346, 6144, 1.0f },
    { 347, 6144, 1.0f },
    { 348, 6144, 1.0f },
    { 349, 4096, 1.0f },
    { 350, 6144, 1.0f },
};

static struct PreRender D54E8[8] = {
    { "STORY MODE", "w", 256, 256, 0, 100, 39 },
    { "FIGHT", "fight", 160, 112, 0, 3, 0 },
    { "DOGFIGHT", "dogfight", 160, 112, 0, 3, 0 },
    { "TENNIS", "tennis", 160, 112, 0, 3, 0 },
    { "BASEBALL", "baseball", 160, 112, 0, 3, 0 },
    { "BANNER", "banner", 96, 32, 0, 421, 28 },
    { "REPLIST", "replist", 400, 112, 0, 421, 20 },
    { "PRACTICE", "replist", 160, 112, 0, 421, 20 },
};

void fn_8_B994(void)
{
    struct PreRender *v30;
    int tr3;
    u32 v29;

    g_font_char_var_3 = 0;
    b5EA = 1;
    fn_1_73900(b5EA);
    load_stage(b5EA);
    modeCtrl.gameType = 0;
    modeCtrl.unk30 = 1;
    dest_all_events();
    event_init(0);
    event_init(4);
    event_init(0xE);
    event_init(0x14);
    event_init(0x10);
    event_init(5);
    event_init(0x11);
    enable_cameras_for_active_players();
    camera_set_state(0xC);
    g_poolInfo.playerPool.statusList[0] = 2;
    g_poolInfo.playerPool.statusList[1] = 0;
    g_poolInfo.playerPool.statusList[2] = 0;
    g_poolInfo.playerPool.statusList[3] = 0;
    b5EC = 0;
    v30 = D54E8;
    v29 = 0;
    while (v29 < 8) {
        tr3 = GXGetTexBufferSize(v30->w, v30->h, 5, 0, 0);
        if (tr3 > b5EC) {
            b5EC = tr3;
        }
        v29++;
        v30++;
    }
    b5F0 = alloc_from_heap_or_panic(currentHeap, b5EC, "test_mode.c", 0x1AB6);
    modeCtrl.submodeTimer = 0;
    modeCtrl.unk10 = 0;
    modeCtrl.courseFlags &= 0xFFFFFFFB;
    set_sub_mode_destination(fn_8_C2A0);
    sub_mode_request = 0x96;
}

void fn_8_BB78(void)
{
    char sp8[0x100];
    int v30;
    struct PreRender *tr29;
    int v3;
    void *fp;
    int v28;
    int tr4;
    int v27;
    int tr3;
    struct PreRender *tr27;

    if ((u32) (modeCtrl.courseFlags & 4) == 0) {
        v3 = modeCtrl.unk10;
        if (PAD_REPEAT(8)) {
            v3--;
            if (v3 < 0) {
                v3 = 7;
            }
        }
        if (PAD_REPEAT(4)) {
            v3++;
            if ((u32) v3 >= 8) {
                v3 = 0;
            }
        }
        modeCtrl.unk10 = v3;
        if (PAD[2].button & 0x100) {
            tr29 = &D54E8[modeCtrl.unk10];
            if ((u32) (tr29->unk14 & 8) != 0) {
                v28 = tr29->w * 2;
                v27 = tr29->h * 2;
            } else {
                v28 = tr29->w;
                v27 = tr29->h;
            }
            for (v30 = 0; v30 < 4; v30++) {
                set_camera_view_settings(v30, ((0x270 - v28) / 640.0), ((0x1B0 - v27) / 448.0), (v28 / 640.0), (v27 / 448.0));
            }
            b5EA = (s16) tr29->unkC;
            dest_all_events();
            if ((u32) (tr29->unk14 & 2) != 0) {
                v28 = D5150[b5EA].unk0;
            } else {
                v28 = b5EA;
            }
            fn_1_73900(v28);
            load_stage(v28);
            if ((u32) (tr29->unk14 & 1) != 0) {
                event_init(0);
                event_init(0x10);
                event_init(0x11);
                enable_cameras_for_active_players();
                camera_set_state(0x48);
            } else {
                fn_1_73900(b5EA);
                event_init(0);
                event_init(4);
                event_init(0xE);
                event_init(0x14);
                event_init(0x10);
                event_init(5);
                event_init(0x11);
                enable_cameras_for_active_players();
                if ((u32) (tr29->unk14 & 0x10) != 0) {
                    camera_set_state(0x5B);
                } else {
                    camera_set_state(0xC);
                }
            }
            modeCtrl.courseFlags |= 4;
            modeCtrl.submodeTimer = 0;
        }
    } else {
        g_font_char_var_3 = 0;
        dest_all_events();
        tr27 = &D54E8[modeCtrl.unk10];
        switch (modeCtrl.submodeTimer & 0xF) {
        case 10:
            tr3 = GXGetTexBufferSize(tr27->w, tr27->h, 5, 0, 0);
            DCFlushRange(b5F0, tr3);
            if ((u32) (tr27->unk14 & 2) == 0) {
                sprintf(sp8, "preview/%dx%d/st%03d.gct", tr27->w, tr27->h, b5EA);
            } else {
                sprintf(sp8, "preview/%dx%d/w%02d.gct", tr27->w, tr27->h, b5EA);
            }
            fp = fn_8006F330(sp8, "wb");
            if (fp == NULL) {
                printf("Cannot fopen:%s\n", sp8);
            } else {
                fwrite(b5F0, (tr3 + 0x1F) & 0xFFFFFFE0, 1, fp);
                fclose(fp);
            }
            break;
        case 0:
            g_some_other_flags |= 2;
            break;
        case 15:
            g_some_other_flags &= 0xFFFFFFFD;
            tr4 = b5EA;
            tr4++;
            if (tr4 >= tr27->unk10) {
                dest_all_sprites();
                modeCtrl.courseFlags &= 0xFFFFFFFB;
            } else {
                b5EA = tr4;
                if ((u32) (tr27->unk14 & 2) != 0) {
                    v28 = D5150[tr4].unk0;
                } else {
                    v28 = tr4;
                }
                fn_1_73900(v28);
                load_stage(v28);
                if (((u32) (tr27->unk14 & 1) != 0) && (is_bonus_stage(v28) != 0)) {
                    s_lightPool = 0;
                }
                GXInvalidateTexAll();
            }
            break;
        }
        if ((u32) (tr27->unk14 & 1) != 0) {
            event_init(0);
            event_init(0x10);
            event_init(0x11);
            enable_cameras_for_active_players();
            camera_set_state(0x48);
        } else {
            fn_1_73900(b5EA);
            event_init(0);
            event_init(4);
            event_init(0xE);
            event_init(0x14);
            event_init(0x10);
            event_init(5);
            event_init(0x11);
            enable_cameras_for_active_players();
            if ((u32) (tr27->unk14 & 0x10) != 0) {
                camera_set_state(0x5B);
            } else {
                camera_set_state(0xC);
            }
        }
        if (PAD[2].button & 0x200) {
            dest_all_sprites();
            modeCtrl.courseFlags &= 0xFFFFFFFB;
        }
        modeCtrl.submodeTimer++;
    }
}

void fn_8_C2A0(void)
{
    switch (sub_mode) {
    case 0x95:
    case 0x96:
        set_sub_mode_destination(fn_8_C2A0);
        return;
    }
    OSFreeToHeap(currentHeap, b5F0);
    GXSetPixelFmt(0, 0);
}

void fn_8_C314(void)
{
    int v31;
    struct PreRender *tr29;
    int v28;
    struct PreRender *v29;
    s8 stageState;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("PRREND TEST");
    g_some_render_flag |= 0x800;
    v29 = D54E8;
    v28 = 0;
    while ((u32) v28 < 8) {
        if (v28 == modeCtrl.unk10) {
            debug_textdraw_set_pos(2, v28 + 3);
            debug_textdraw_set_color(0x33);
            debug_textdraw_put_buf_1("\034");
            debug_textdraw_set_color(0xFF);
        }
        debug_textdraw_set_pos(3, v28 + 3);
        debug_textdraw_vsprintf_buf_1("[ %3dx%3d ] %s", v29->w, v29->h, v29->en);
        v28++;
        v29++;
    }
    if ((u32) (modeCtrl.courseFlags & 4) != 0) {
        tr29 = &D54E8[modeCtrl.unk10];
        if ((u32) (tr29->unk14 & 8) != 0) {
            v31 = tr29->w * 2;
            v28 = tr29->h * 2;
        } else {
            v31 = tr29->w;
            v28 = tr29->h;
        }
        if ((u32) (tr29->unk14 & 1) != 0) {
            GXColor black = {0, 0, 0, 0};

            GXSetPixelFmt(1, 0);
            GXSetCopyClear_cached(black, 0xFFFFFF);
        } else {
            GXSetPixelFmt(0, 0);
            GXSetCopyClear_cached(world_theme.backdropColor, 0xFFFFFF);
        }
        if ((u32) (tr29->unk14 & 0x20) != 0) {
            fn_1_D462C();
            init_shadow_reqs();
        }
        camera_apply_viewport_if_changed(0);
        if ((u32) (tr29->unk14 & 2) != 0) {
            fn_8_C74C(tr29->w, tr29->h);
        }
        fn_1_D4870();
        g_maybe_some_bg_effect_handler();
        u_reset_light_group_stack(0);
        stageState = events[EVENT_STAGE].state;
        if (stageState == 2 || stageState == 4) {
            g_draw_stage();
        }
        if ((u32) (tr29->unk14 & 4) == 0) {
            switch (current_stage_id) {
            case 0x8F:
            case 0x90:
            case 0x91:
                break;
            default:
                mtxa_from_mtxb();
                mtxa_translate(&stagedef->startPos->pos);
                mtxa_rotate_y((s16) (g_font_char_var_3 << 9));
                load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
                avdisp_draw_model_culled_sort_auto(init_common_gma->modelEntries[77].model);
                break;
            }
        }
        draw_shadow_reqs();
        if (events[EVENT_BACKGROUND].state == 2) {
            g_smth_with_rendefc_reflective_height(400.000000f);
            g_draw_bg();
            g_smth_with_rendefc_reflective_height(0.0f);
        }
        if (events[EVENT_ITEM].state == 2) {
            draw_items();
        }
        if (events[EVENT_STOBJ].state == 2) {
            g_draw_stobjs();
        }
        if (events[EVENT_EFFECT].state == 2) {
            g_draw_effects();
        }
        ord_tbl_draw_nodes();
        if ((u32) (tr29->unk14 & 8) != 0) {
            GXSetTexCopySrc_cached((u16) (0x270 - v31), (u16) (0x1B0 - v28), v31, v28);
            GXSetTexCopyDst_cached((u16) (v31 >> 1), (u16) (v28 >> 1), 5, 1);
            GXCopyTex(b5F0, 0);
        } else {
            GXSetTexCopySrc_cached((u16) (0x270 - v31), (u16) (0x1B0 - v28), v31, v28);
            GXSetTexCopyDst_cached(v31, v28, 5, 0);
            GXCopyTex(b5F0, 0);
        }
    }
    camera_setup_default_projection();
    g_some_render_flag &= 0xFFFFF7FF;
}

void fn_8_C74C(float width, float height)
{
    Mtx44 proj;
    float projv[7];
    Vec eye;
    Vec target;
    Vec up;
    struct Sphere sphere;
    float scale;

    C_MTXPerspective(proj, 59.996338f, 1.3333334f, 0.1f, 20000.000000f);
    g_something_with_GXSetProjectionv(proj, 0);
    g_something_with_GXSetProjectionv(proj, 0);
    GXGetProjectionv(projv);
    projv[1] *= 640.000000f / width;
    projv[3] *= 480.000000f / height;
    GXSetProjectionv(projv);
    get_curr_stage_fly_in_position(&sphere);
    eye.x = 0.0f;
    eye.y = 56.0f;
    eye.z = 64.0f;
    target.x = 0.0f;
    target.y = 24.0f;
    target.z = 0.0f;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    scale = D5150[b5EA].unk4;
    C_MTXLookAt(mtxa->mtxA, &eye, &up, &target);
    mtxa_translate_xyz(0.0f, 24.0f, 0.0f);
    mtxa_rotate_y(D5150[b5EA].unk2);
    mtxa_scale_s((36.0f * scale) / sphere.radius);
    mtxa_translate_neg(&sphere.pos);
    mtxa_to_mtx(mtxa->mtxB);
}

void fn_8_C8F8(s8 *status, struct Sprite *sprite)
{
    g_load_sprite_tex_from_dvd(sprite->tex);
}

void fn_8_C91C(struct Sprite *arg0)
{
    fn_1_F9EAC(arg0->tex, arg0->x, arg0->y, arg0->depth, arg0->scaleX, arg0->scaleY, -1, 0);
}

void fn_8_C960(struct Sprite *arg0)
{
    fn_1_F9E1C(arg0->tex);
}

void fn_8_C984(void)
{
    event_init(0x11);
    modeCtrl.unk10 = 0;
    modeCtrl.unk44 = 1;
    modeCtrl.courseFlags &= 0xFFFFFFFB;
    sub_mode_request = 0x98;
}

void fn_8_C9EC(void)
{
    char sp8[0x100];
    struct Sprite *tr3;
    struct PreRender *tr30;
    struct PreRender *tr4;
    int v30;
    int v3;

    if ((u32) (modeCtrl.courseFlags & 4) == 0) {
        v3 = modeCtrl.unk10;
        if (PAD_REPEAT(8)) {
            v3--;
            if (v3 < 0) {
                v3 = 7;
            }
        }
        if (PAD_REPEAT(4)) {
            v3++;
            if ((u32) v3 >= 8) {
                v3 = 0;
            }
        }
        modeCtrl.unk10 = v3;
        if (PAD[2].button & 0x100) {
            tr30 = &D54E8[modeCtrl.unk10];
            modeCtrl.unk44 = tr30->unkC;
            tr3 = create_sprite();
            if (tr3 != NULL) {
                sprintf(sp8, "preview/%s%dx%d.tpl", tr30->jp, tr30->w, tr30->h);
                g_load_preview_texture(b5F4, sp8, modeCtrl.unk44 - tr30->unkC, tr30->w, tr30->h, 5);
                tr3->tex = (struct SpriteTexture *) b5F4;
                tr3->x = (624.0 - tr30->w);
                tr3->y = (464.0 - tr30->h);
                tr3->scaleX = tr30->w;
                tr3->scaleY = tr30->h;
                tr3->mainFunc = fn_8_C8F8;
                tr3->drawFunc = fn_8_C91C;
                tr3->destFunc = fn_8_C960;
            }
            modeCtrl.courseFlags |= 4;
        }
    } else {
        tr4 = &D54E8[modeCtrl.unk10];
        v30 = modeCtrl.unk44;
        if (PAD_REPEAT(1)) {
            v30--;
            if (v30 < tr4->unkC) {
                v30 = tr4->unk10 - 1;
            }
        }
        if (PAD_REPEAT(2)) {
            v30++;
            if (v30 >= tr4->unk10) {
                v30 = tr4->unkC;
            }
        }
        if (v30 != modeCtrl.unk44) {
            fn_1_F9CAC(b5F4, v30 - tr4->unkC);
            modeCtrl.unk44 = v30;
        }
        if (PAD[2].button & 0x200) {
            dest_all_sprites();
            modeCtrl.courseFlags &= 0xFFFFFFFB;
        }
    }
}

/* Unreferenced in the shipped REL; only its string literals survive. */
void preview_test_unused(struct PreRender *pr) {
    debug_textdraw_put_buf_1("PREVIEW TEST");
    debug_textdraw_vsprintf_buf_1(" [ %3d / %3d ]", modeCtrl.unk44 - pr->unkC, pr->unk10 - pr->unkC);
}

void fn_8_CEB4(void)
{
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        g_set_up_debugtext_buffer();
        sub_mode_request = 0x89;
        dest_all_events();
        fn_1_56C18();
        load_stage(g_some_stage_id);
        event_init(0);
        event_init(1);
        event_init(2);
        event_init(3);
        event_init(4);
        event_init(9);
        event_init(5);
        event_init(7);
        event_init(0x10);
        event_init(0x11);
        event_init(0x13);
        event_init(0xE);
        event_init(0x15);
        event_init(0x14);
        enable_cameras_for_active_players();
        camera_set_state(2);
        lbl_1_bss_71EC8.unk14 = 0;
        b66C = lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14];
        b670 = 0;
    }
}

void fn_8_CFDC(void)
{
    struct ReplayHeader sp8;
    struct World *v7;
    u32 size;
    struct Ball *v4;
    struct Ball *saved;
    void *fp;
    s8 *v5;
    struct Ball *tr30;
    void *buf;
    void *tr3;
    int v6;
    int v27;

    tr30 = current_ball;
    if ((u32)(g_some_other_flags & 0xA) == 0) {
        g_set_up_debugtext_buffer();
        if (((PAD[2].button & 1) || ((PAD[0].button & 1) && (PAD[0].button & 0x20))) && (b66C > 0)) {
            b66C = b66C - 1;
        }
        if (((PAD[2].button & 2) || ((PAD[0].button & 2) && (PAD[0].button & 0x20))) && (b66C < 0x11)) {
            b66C = b66C + 1;
        }
        fn_1_62AA0(b66C, &sp8);
        debug_textdraw_set_pos(8, 4);
        debug_textdraw_vsprintf_buf_1(" REPLAY TEST\n\n");
        if (b66C != 0x11) {
            debug_textdraw_vsprintf_buf_1("     ID: %03d\n", b66C);
        } else {
            debug_textdraw_set_color(0xC3);
            debug_textdraw_vsprintf_buf_1("     Record for Save\n");
            debug_textdraw_set_color(0xFF);
        }
        if ((sp8.flags & 0x200) || (fn_1_61B18(b66C) == 0.0f)) {
            debug_textdraw_set_color(0xC3);
            debug_textdraw_vsprintf_buf_1("       :NO DATA\n");
            debug_textdraw_set_color(0xFF);
        } else {
            debug_textdraw_vsprintf_buf_1("  STAGE: %03d(Lib No.%03d)\n", sp8.floor, sp8.stageId);
            debug_textdraw_vsprintf_buf_1(" COURSE: %3d\n", sp8.difficulty);
            if (PAD[0].button & 0x800) {
                debug_textdraw_vsprintf_buf_1("  MONKY: %3d\n", sp8.monkey);
                debug_textdraw_vsprintf_buf_1("   NAME: %s\n", sp8.name);
                debug_textdraw_vsprintf_buf_1("  GRADE: %f\n", sp8.grade);
            }
        }
        debug_textdraw_set_pos(8, 0x20);
        debug_textdraw_set_color(0xF3);
        debug_textdraw_vsprintf_buf_1("[ USAGE ]\n");
        debug_textdraw_set_color(0xFF);
        debug_textdraw_vsprintf_buf_1("L + B :Save Current Replay\n");
        if (tr30->state == 4) {
            mode_info.flags &= ~0x810;
        }
        switch (b670) {
        case 0:                                     
            if (((sp8.flags & 0x200) == 0) && (fn_1_61B18(b66C) > 0.0) && (PAD[2].button & 0x100) && ((PAD[2].button & 0x200) == 0)) {
                lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14] = b66C;
                current_stage_id = (s16) sp8.stageId;
                modeCtrl.difficulty = sp8.difficulty;
                event_dest(0xE);
                fn_1_618F4(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
                mode_info.flags |= 0x10;
                load_stage(current_stage_id);
                event_dest(0);
                event_dest(4);
                event_dest(5);
                event_dest(0xE);
                event_dest(0x15);
                event_dest(0x14);
                event_dest(2);
                event_dest(3);
                event_init(0);
                event_init(4);
                event_init(5);
                event_init(0xE);
                event_init(0x15);
                event_init(0x14);
                event_init(2);
                event_init(3);
                saved = current_ball;
                v4 = balls;
                v6 = 0;
                while (v6 < 8) {
                    if (v4->status == 2) {
                        current_ball = v4;
                        v4->state = 9;
                    }
                    v6++;
                    v4++;
                }
                current_ball = saved;
                v7 = world_infos;
                v5 = g_poolInfo.playerPool.statusList;
                v6 = 0;
                while (v6 < g_poolInfo.playerPool.count) {
                    if (*v5 == 2) {
                        v7->state = 6;
                    }
                    v6++;
                    v7++;
                    v5++;
                }
                enable_cameras_for_active_players();
                camera_set_state(0x2C);
                lbl_1_bss_71EC8.unk10 = fn_1_61B18(lbl_1_bss_71EC8.unk0[lbl_1_bss_71EC8.unk14]);
                g_advance_stage_animation();
            }
            if (((sp8.flags & 0x200) == 0) && (PAD[2].button & 0x100) && (PAD[0].button & 0x40)) {
                saved = current_ball;
                v4 = balls;
                v6 = 0;
                while (v6 < 8) {
                    if (v4->status == 2) {
                        current_ball = v4;
                        v4->state = 0;
                    }
                    v6++;
                    v4++;
                }
                current_ball = saved;
                v7 = world_infos;
                v5 = g_poolInfo.playerPool.statusList;
                v6 = 0;
                while (v6 < g_poolInfo.playerPool.count) {
                    if (*v5 == 2) {
                        v7->state = 4;
                    }
                    v6++;
                    v7++;
                    v5++;
                }
                fn_1_5FE5C(b66C);
            }
            if ((PAD[2].button & 0x200) && (PAD[0].button & 0x40)) {
                b670 = 1;
                modeCtrl.submodeTimer = 0;
                return;
            }
            break;
        case 1:                                     
            debug_textdraw_set_pos(0x10, 0x18);
            debug_textdraw_vsprintf_buf_1("SAVE This Replay Data\n");
            switch (modeCtrl.submodeTimer) { 
            case 0:                                 
                modeCtrl.submodeTimer = 1;
                return;
            case 1:                                 
                modeCtrl.submodeTimer = 2;
                return;
            case 2:                                 
                tr3 = fn_1_6333C(b66C);
                size = fn_1_63354(tr3);
                buf = alloc_from_heap_or_panic(currentHeap, size, "test_mode.c", 0x1E03);
                fn_1_6335C(tr3, buf);
                v27 = 0;
                sprintf(s_rep.path, "replay/%03X%1X.%04X", sp8.stageId & 0xFFF, v27, sp8.flags & 0xFFFF);
                while ((fp = fn_8006F330(s_rep.path, "rb")) != NULL) {
                    fclose(fp);
                    sprintf(s_rep.path, "replay/%03X%1X.%04X", sp8.stageId & 0xFFF, v27, sp8.flags & 0xFFFF);
                    v27++;
                }
                fp = fn_8006F330(s_rep.path, "wb");
                if (fp == NULL) {
                    modeCtrl.submodeTimer = 4;
                } else {
                    fwrite(buf, 1, size, fp);
                    fclose(fp);
                    modeCtrl.submodeTimer = 3;
                }
                OSFreeToHeap(currentHeap, buf);
                return;
            case 3:                                 
                debug_textdraw_vsprintf_buf_1("[OK]\n");
                debug_textdraw_vsprintf_buf_1("Save Replay as ");
                debug_textdraw_set_color(0xC3);
                debug_textdraw_vsprintf_buf_1("%s\n", s_rep.path);
                debug_textdraw_set_color(0xFF);
                debug_textdraw_vsprintf_buf_1("Press Button A\n");
                if (PAD[2].button & 0x100) {
                    b670 = 0;
                    return;
                }
                break;
            case 4:                                 
                debug_textdraw_vsprintf_buf_1("[ERROR]\n");
                debug_textdraw_set_color(0xC3);
                debug_textdraw_vsprintf_buf_1("%s\n", s_rep.path);
                debug_textdraw_vsprintf_buf_1("Run Program in CodeWarrior IDE!\n");
                debug_textdraw_set_color(0xFF);
                debug_textdraw_vsprintf_buf_1("Press Button A\n");
                if (PAD[2].button & 0x100) {
                    b670 = 0;
                }
                break;
            }
            break;
        }
    }
}

static char *D57BC[3] = {
    "Normal", "Extra No Continue", "No Mistake",
};

void fn_8_D97C(void)
{
    b774 = 0;
    b776 = 0;
    b778 = 0;
    b77A = 0;
    b77C = 0;
    fn_1_141288();
    fn_1_142ECC();
    load_scene_files(b77A);
    fn_1_143798(0x191);
    g_load_efcmdl_gmatpl();
    OSReport("Test Sceneplay Initialize.\n");
    set_sub_mode_destination(fn_8_E088);
    modeCtrl.courseFlags |= 0x04000000;
    sub_mode_request = 0xA0;
}

void fn_8_DA2C(void)
{
    struct ExternalCameraPose *pose;

    switch (b77C) {
    case 0:                                         /* switch 1 */
        if (PAD[2].button & 2) {
            b774++;
            if (b774 > lbl_1_bss_DD4EC) {
                b774 = 0;
            }
        } else if (PAD[2].button & 1) {
            b774--;
            if (b774 < 0) {
                b774 = (s16) lbl_1_bss_DD4EC;
            }
        } else if (PAD[2].button & 8) {
            b77C = 2;
        } else if (PAD[2].button & 4) {
            b77C = 1;
        }
        if (((PAD[2].button & 0x100) && ((u32) (g_some_other_flags & 1) == 0)) || ((PAD[0].button & 0x20) && (PAD[2].button & 0x100) && ((u32) (g_some_other_flags & 1) != 0))) {
            fn_1_141370(b774, 0);
            b776 = 0;
        }
        if (((PAD[2].button & 0x800) && ((u32) (g_some_other_flags & 1) == 0)) || ((PAD[0].button & 0x20) && (PAD[2].button & 0x800) && ((u32) (g_some_other_flags & 1) != 0))) {
            fn_1_141370(b774, 0);
            b776 = b774;
            fn_1_D3930();
        }
        if (PAD[2].button & 0x200) {
            g_dump_scene_info(b774);
        }
        break;
    case 1:                                         /* switch 1 */
        if (PAD[2].button & 2) {
            b77A++;
            if (b77A >= 0x10) {
                b77A = 0;
            }
        } else if (PAD[2].button & 1) {
            b77A--;
            if (b77A < 0) {
                b77A = 0xF;
            }
        } else if (PAD[2].button & 8) {
            b77C = 0;
        } else if (PAD[2].button & 4) {
            b77C = 2;
        }
        if ((PAD[2].button & 0x100) && ((u32) (g_some_other_flags & 1) == 0)) {
            g_something_freeing_heap_4();
            fn_1_142F00();
            g_something_with_cutscenes_func2();
            b774 = 0;
            b776 = 0;
            load_scene_files(b77A);
        }
        break;
    case 2:                                         /* switch 1 */
        if (PAD[2].button & 2) {
            b778++;
            if (b778 >= 3) {
                b778 = 0;
            }
        } else if (PAD[2].button & 1) {
            b778--;
            if (b778 < 0) {
                b778 = 2;
            }
        } else if (PAD[2].button & 8) {
            b77C = 1;
        } else if (PAD[2].button & 4) {
            b77C = 0;
        }
        break;
    }
    if (((PAD[2].button & 0x200) && ((u32) (g_some_other_flags & 1) == 0)) || (PAD[2].button & 0x400)) {
        g_something_freeing_heap_4();
        b776 = 0;
        get_load_group_status();
    }
    pose = &lbl_1_bss_DD4F4;
    if (b776 != 0 && pose->frame >= pose->frameMax) {
        if (++b776 > lbl_1_bss_DD4EC || pose->category != g_something_with_cutscenes_func(b776)) {
            b776 = 0;
        } else {
            printf("================== Set_Scene( %d ) =====================\n", b776);
            fn_1_141370(b776, 0);
        }
    }
    modeCtrl.courseFlags &= 0xFF7FFFA7;
    mode_info.livesLost = 0;
    switch (b778) {
    case 0:                                         /* switch 2 */
        mode_info.livesLost = 1;
        break;
    case 1:
        switch (g_active_scene_category) {
        case 12:
        case 13:
        case 14:
            modeCtrl.courseFlags |= 8;
            break;
        case 15:
            modeCtrl.courseFlags |= 0x800000 | 0x10;
            break;
        }
        break;
    case 2:
        switch (g_active_scene_category) {
        case 12:
        case 13:
            modeCtrl.courseFlags |= 0x48;
            break;
        case 14:
            break;
        case 15:
            modeCtrl.courseFlags |= 0x800000 | 0x50;
            break;
        }
        break;
    }
    fn_1_141E48();
}

void fn_8_E088(void)
{
    switch (sub_mode) {
    case 0x9F:
    case 0xA0:
        set_sub_mode_destination(fn_8_E088);
        return;
    }
    g_something_freeing_heap_4();
    fn_1_142F00();
    g_something_with_cutscenes_func2();
    unload_stage();
    fn_1_1118BC();
    modeCtrl.courseFlags &= 0xFBFFFFFF;
    g_something_freeing_heap_parent();
}

static char *D58A4[16] = {
    "SCENE1A", "SCENE1B", "SCENE2", "SCENE3",
    "SCENE4", "SCENE5", "SCENE6", "SCENE7",
    "SCENE8", "SCENE9", "SCENE10", "SCENELAST",
    "ENDING1", "ENDING2", "ENDING3", "ENDING4",
};

void fn_8_E100(void)
{
    char sp8[0x40];
    float tf31;
    struct ExternalCameraPose *pose;

    if ((PAD[0].button & 0x20) == 0) {
        pose = &lbl_1_bss_DD4F4;
        g_something_with_cutscenes_func(b774);
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_vsprintf_buf_1("SCENE PLAY TEST\n\n");
        if (b77C == 0) {
            debug_textdraw_set_color(0x33);
        }
        debug_textdraw_vsprintf_buf_1("test_scn_num  %2d / %2d : %s\n", b774, lbl_1_bss_DD4EC, fn_1_14319C(b774));
        debug_textdraw_set_color(0xFF);
        if (b77C == 1) {
            debug_textdraw_set_color(0x33);
        }
        debug_textdraw_vsprintf_buf_1("test_category %2d / %2d : %s\n", b77A, 15, D58A4[b77A]);
        debug_textdraw_set_color(0xFF);
        if (b77C == 2) {
            debug_textdraw_set_color(0x33);
        } else {
            switch (b77A) {
            case 12:
            case 13:
            case 14:
            case 15:
                debug_textdraw_set_color(0xFF);
                break;
            default:
                debug_textdraw_set_color(0x57);
                break;
            }
        }
        debug_textdraw_vsprintf_buf_1("    condition %2d / %2d : %s\n\n", b778 + 1, 3, D57BC[b778]);
        debug_textdraw_set_color(0xFF);
        debug_textdraw_vsprintf_buf_1("category      %2d / %2d : %s\n", g_active_scene_category, 15, D58A4[g_active_scene_category]);
        debug_textdraw_vsprintf_buf_1("play_scn_num %5d", b776);
        debug_textdraw_vsprintf_buf_1(" : %s\n", fn_1_14319C(b776));
        debug_textdraw_vsprintf_buf_1("scene_num    %5d\n", pose->sceneNum);
        debug_textdraw_vsprintf_buf_1("scene_length %5d\n", pose->frameMax);
        debug_textdraw_vsprintf_buf_1("scene_coma   %5d\n", pose->frame);
        debug_textdraw_vsprintf_buf_1("stage        %5d\n\n", fn_1_143104(b774));
        tf31 = get_gx_fifo_use_size() / 1048576.0f;
        if (tf31 > 0.8f && (u32)(g_font_char_var_3 & 1) != 0) {
            debug_textdraw_set_color(0xC3);
            sprintf(sp8, "-- WARNING!! --");
            debug_textdraw_set_pos((u32)(0x35 - strlen(sp8)) >> 1, 0x12);
            debug_textdraw_vsprintf_buf_1("%s", sp8);
            sprintf(sp8, "[ GRAPHIC FIFO (%3.1f%%) ]", 100.0f * tf31);
            debug_textdraw_set_pos((u32)(0x35 - strlen(sp8)) >> 1, 0x13);
            debug_textdraw_vsprintf_buf_1("%s", sp8);
            debug_textdraw_set_color(0xFF);
        }
    }
    fn_1_1439F4();
}

void fn_8_E428(void)
{
    Vec pos;
    int v31;
    struct DialogBox *box;
    struct Ape **v30;
    struct Ape *ape;

    dest_all_events();
    event_init(0x10);
    event_init(3);
    event_init(0x11);
    enable_cameras_for_active_players();
    camera_set_state(2);
    s_dlg.category = 0;
    s_dlg.index = 0;
    s_dlg.box = 0;
    s_dlg.ape = 0;
    s_dlg.tbl = load_dialog_tbl(s_dlg.category);
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    v30 = s_dlg.apes;
    for (v31 = 0; v31 < 6; v31++, v30++) {
        ape = g_load_ape_gameplay_variant0(v31 % 4, 2);
        *v30 = ape;
        mtxa_from_translate(&pos);
        mtxa_to_quat(&ape->rot);
        mathutil_mtxA_get_translate(&ape->pos);
        pos.x += 1.0f;
    }
    box = s_dlg.boxes;
    for (v31 = 3; v31 > 0; v31--, box++) {
        fn_1_15FB34(box, 0x100);
    }
    box = &s_dlg.boxes[s_dlg.box];
    fn_1_160518(box, g_smth_with_dialog2(s_dlg.tbl, s_dlg.index));
    fn_1_1607DC(box, s_dlg.apes[s_dlg.ape]);
    fn_1_1607E4(box);
    fn_1_160870(box);
    set_sub_mode_destination(fn_8_EA44);
    sub_mode_request = 0xA2;
}

void fn_8_E674(void)
{
    struct DialogBox *box;
    int v30;
    int tr4;
    struct Ape **v31;

    if ((u32)(g_some_other_flags & 0xA) == 0) {
        v30 = s_dlg.category;
        if (PAD[2].button & 2) {
            v30++;
            if (v30 >= 0x11) {
                v30 = 0;
            }
        }
        if (PAD[2].button & 1) {
            v30--;
            if (v30 < 0) {
                v30 = 0x10;
            }
        }
        if (v30 != s_dlg.category) {
            fn_1_162978(s_dlg.tbl);
            s_dlg.tbl = load_dialog_tbl(v30);
            s_dlg.category = (s16)v30;
            s_dlg.index = 0;
            fn_1_1607F4(&s_dlg.boxes[s_dlg.box], 0);
            s_dlg.box--;
            if (s_dlg.box < 0) {
                s_dlg.box = 2;
            }
            box = &s_dlg.boxes[s_dlg.box];
            fn_1_160518(box, g_smth_with_dialog2(s_dlg.tbl, s_dlg.index));
            fn_1_1607DC(box, s_dlg.apes[s_dlg.ape]);
            fn_1_1607E4(box);
            fn_1_160870(box);
        }
        tr4 = *s_dlg.tbl;
        v30 = s_dlg.index;
        if (PAD[2].button & 4) {
            v30++;
            if (v30 >= tr4) {
                v30 = 0;
            }
        }
        if (PAD[2].button & 8) {
            v30--;
            if (v30 < 0) {
                v30 = tr4 - 1;
            }
        }
        if (v30 != s_dlg.index) {
            fn_1_1607F4(&s_dlg.boxes[s_dlg.box], 0);
            s_dlg.ape--;
            if (s_dlg.ape < 0) {
                s_dlg.ape = 5;
            }
            s_dlg.box--;
            if (s_dlg.box < 0) {
                s_dlg.box = 2;
            }
            box = &s_dlg.boxes[s_dlg.box];
            fn_1_160518(box, g_smth_with_dialog2(s_dlg.tbl, v30));
            fn_1_1607DC(box, s_dlg.apes[s_dlg.ape]);
            fn_1_1607E4(box);
            fn_1_160870(box);
            s_dlg.index = (s16)v30;
        }
        box = s_dlg.boxes;
        for (v30 = 3; v30 > 0; v30--, box++) {
            fn_1_15FB94(box);
        }
        v31 = s_dlg.apes;
        for (v30 = 0; v30 < 6; v30++, v31++) {
            g_ape_anim_body(*v31);
        }
    }
}

void fn_8_EA44(void)
{
    int v31;
    struct DialogBox *box;
    struct Ape **v30;

    switch (sub_mode) {
    case 0xA1:
    case 0xA2:
        set_sub_mode_destination(fn_8_EA44);
        return;
    }
    box = s_dlg.boxes;
    for (v31 = 3; v31 > 0; v31--, box++) {
        fn_1_15FB70(box);
    }
    fn_1_162978(s_dlg.tbl);
    v30 = s_dlg.apes;
    for (v31 = 0; v31 < 6; v31++, v30++) {
        g_something_with_freeing_chara_heap_ape(*v30);
        *v30 = NULL;
    }
}

void fn_8_EB10(void)
{
    s8 tr3;
    int v29;
    int v31;
    struct Ball *tr30;
    struct Ape **v28;

    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("DIALOG TEST\n");
    debug_textdraw_set_pos(6, 6);
    debug_textdraw_vsprintf_buf_1("Category [%d/%d] %s\n", s_dlg.category + 1, 0x11, lbl_1_data_70D98[s_dlg.category]);
    debug_textdraw_vsprintf_buf_1("    Data [%d/%d]\n", s_dlg.index + 1, *s_dlg.tbl);
    tr30 = current_ball;
    for (v31 = 0; v31 < 5; v31++) {
        if (cameras[v31].sub28.viewportW > 0.0f && cameras[v31].sub28.viewportH > 0.0f) {
            tr3 = g_poolInfo.playerPool.statusList[v31];
            if ((tr3 != 0 && tr3 != 4 && (cameras[v31].flags & 0x40) == 0) || (cameras[v31].flags & 0x80)) {
                current_ball = &balls[v31];
                camera_apply_viewport_if_changed(v31);
                fn_1_D4870();
                g_maybe_some_bg_effect_handler();
                u_reset_light_group_stack(v31);
                v28 = s_dlg.apes;
                for (v29 = 0; v29 < 6; v29++, v28++) {
                    draw_ape(*v28);
                }
                handle_test_camera();
                ord_tbl_draw_nodes();
            }
        }
    }
    current_ball = tr30;
    camera_setup_default_projection();
}

void fn_8_ED34(void)
{
    memset(&s_pass, 0, 4);
    sub_mode_request = 0xA4;
}

void fn_8_ED74(void)
{
    int v4;

    v4 = s_pass.scroll;
    if ((PAD[0].button & 1) || (analog_inputs[0][0] & 1)) {
        v4--;
        if (v4 < 0) {
            v4 = 0;
        }
    } else if ((PAD[0].button & 2) || (analog_inputs[0][0] & 2)) {
        v4++;
        if (v4 > 0x8A) {
            v4 = 0x8A;
        }
    }
    s_pass.scroll = (s16)v4;
    v4 = s_pass.mode;
    if (PAD[2].button & 0x100) {
        v4++;
        if (v4 >= 4) {
            v4 = 0;
        }
    }
    s_pass.mode = (s16)v4;
}

static struct Dummy5ABC D5ABC[9] = {
    { "BEGINNER", (u16 *)&lbl_1_bss_D1320[0x7e8], (u16 *)&lbl_1_bss_D1320[0x7d4], 10, 10, 0, 0, 0, 0 },
    { "BE EX   ", (u16 *)&lbl_1_bss_D1320[0x810], (u16 *)&lbl_1_bss_D1320[0x7fc], 10, 10, 0, 0, 0, 0 },
    { "ADVANCED", (u16 *)&lbl_1_bss_D1320[0x860], (u16 *)&lbl_1_bss_D1320[0x824], 30, 30, 0, 0, 0, 0 },
    { "BE AD   ", (u16 *)&lbl_1_bss_D1320[0x8b0], (u16 *)&lbl_1_bss_D1320[0x89c], 10, 10, 0, 0, 0, 0 },
    { "EXPERT  ", (u16 *)&lbl_1_bss_D1320[0x928], (u16 *)&lbl_1_bss_D1320[0x8c4], 50, 25, 0, 0, 0, 0 },
    { "EX EX   ", (u16 *)&lbl_1_bss_D1320[0x9a0], (u16 *)&lbl_1_bss_D1320[0x98c], 10, 10, 0, 0, 0, 0 },
    { "MASTER  ", (u16 *)&lbl_1_bss_D1320[0x9c8], (u16 *)&lbl_1_bss_D1320[0x9b4], 10, 10, 0, 0, 0, 0 },
    { "EX MA   ", (u16 *)&lbl_1_bss_D1320[0x9f0], (u16 *)&lbl_1_bss_D1320[0x9dc], 10, 10, 0, 0, 0, 0 },
    { "LINEAR  ", (u16 *)&lbl_1_bss_D1320[0x3ec], (u16 *)&lbl_1_bss_D1320[0x4], 420, 35, 1, 0, 0, 0 },
};

void fn_8_EE5C(void)
{
    float tf31;
    struct Dummy5ABC *v30;
    u32 v29;
    int v28;
    int v27;
    int v26;
    int tr25;
    int tr24;
    int tr23;
    int tr22;

    v28 = 0;
    v30 = D5ABC;
    for (v29 = 0; v29 < 9; v29++, v30++) {
        debug_textdraw_set_pos(((v28 * 9) + 1) - s_pass.scroll, 0);
        debug_textdraw_vsprintf_buf_1(v30->name);
        for (v27 = 0; v27 < v30->count / v30->rows; v27++) {
            for (v26 = 0; v26 < v30->rows; v26++) {
                tr22 = v30->first + ((v27 * v30->rows) + v26);
                tr24 = v30->cleared[tr22];
                tr25 = v30->failed[tr22];
                tr23 = tr25 + tr24;
                tf31 = 100.0 * ((float)tr24 / (float)tr23);
                debug_textdraw_set_pos((((v27 + v28) * 9) + 1) - s_pass.scroll, v26 + 1);
                switch (s_pass.mode) {
                case 0:
                    debug_textdraw_vsprintf_buf_1("%3d:%3.0f%%", (tr22 + 1) - v30->first, tf31);
                    break;
                case 1:
                    debug_textdraw_vsprintf_buf_1("%3d:%4d", (tr22 + 1) - v30->first, tr23);
                    break;
                case 2:
                    debug_textdraw_vsprintf_buf_1("%3d:%4d", (tr22 + 1) - v30->first, tr25);
                    break;
                case 3:
                    debug_textdraw_vsprintf_buf_1("%3d:%4d", (tr22 + 1) - v30->first, tr24);
                    break;
                }
            }
            debug_textdraw_set_pos((((v27 + v28) * 9) + 1) - s_pass.scroll, 0x24);
            switch (s_pass.mode) {
            case 0:
                debug_textdraw_vsprintf_buf_1("RATE");
                break;
            case 1:
                debug_textdraw_vsprintf_buf_1("PLAYED");
                break;
            case 2:
                debug_textdraw_vsprintf_buf_1("FAILED");
                break;
            case 3:
                debug_textdraw_vsprintf_buf_1("CLEARED");
                break;
            }
        }
        v28 += v30->count / v30->rows;
    }
}

void fn_8_F140(void)
{
    struct ReplaySize *p;

    p = &s_size;
    g_set_up_debugtext_buffer();
    memset(p, 0, 0x14);
    p->next = 0;
    p->maxStage = -1;
    p->minStage = -1;
    sub_mode_request = 0xA6;
}

void fn_8_F1AC(void)
{
    struct ReplaySize *p;
    u32 size;
    u32 limit;

    p = &s_size;
    dest_all_events();
    debug_textdraw_set_pos(4, 4);
    debug_textdraw_vsprintf_buf_1("[ Replay Size Checker ]\n");
    debug_textdraw_set_pos(6, 6);
    debug_textdraw_vsprintf_buf_1("MODE[%d], CNT[%d]\n", p->mode, p->count);
    debug_textdraw_vsprintf_buf_1("Max Size:\n");
    if (p->maxStage < 0) {
        debug_textdraw_vsprintf_buf_1("---\n");
    } else {
        debug_textdraw_vsprintf_buf_1("0x%08X (STAGE NUM:%03d)\n", p->maxSize, p->maxStage);
        size = (p->maxSize + 0x4073) & 0xFFFFE000;
        debug_textdraw_vsprintf_buf_1("MEMORYCARD: 0x%08X(%d Blocks)\n", size, (size + 0x1FFF) >> 0xD);
    }
    debug_textdraw_vsprintf_buf_1("Min Size:\n");
    if (p->minStage < 0) {
        debug_textdraw_vsprintf_buf_1("---\n");
    } else {
        debug_textdraw_vsprintf_buf_1("0x%08X (STAGE NUM:%03d)\n", p->minSize, p->minStage);
        size = (p->minSize + 0x4073) & 0xFFFFE000;
        debug_textdraw_vsprintf_buf_1("MEMORYCARD: 0x%08X(%d Blocks)\n", size, (size + 0x1FFF) >> 0xD);
    }
    if (p->next >= 0) {
        p->mode = p->next;
        p->next = -1;
        p->count = 0;
    }
    modeCtrl.gameType = 0;
    modeCtrl.courseFlags &= 0xFF7FFFE6;
    switch (p->mode) {
    case 0:
        modeCtrl.difficulty = 0;
        modeCtrl.courseFlags |= 1;
        limit = 0xA;
        break;
    case 1:
        modeCtrl.difficulty = 0;
        modeCtrl.courseFlags |= 9;
        limit = 0xA;
        break;
    case 2:
        modeCtrl.difficulty = 1;
        modeCtrl.courseFlags |= 1;
        limit = 0x1E;
        break;
    case 3:
        modeCtrl.difficulty = 1;
        modeCtrl.courseFlags |= 9;
        limit = 0xA;
        break;
    case 4:
        modeCtrl.difficulty = 2;
        modeCtrl.courseFlags |= 1;
        limit = 0x32;
        break;
    case 5:
        modeCtrl.difficulty = 2;
        modeCtrl.courseFlags |= 9;
        limit = 0xA;
        break;
    case 6:
        modeCtrl.difficulty = 2;
        modeCtrl.courseFlags |= 0x19;
        limit = 0xA;
        break;
    case 7:
        modeCtrl.difficulty = 2;
        modeCtrl.courseFlags |= 0x800000 | 0x19;
        limit = 0xA;
        break;
    case 8:
        limit = 0x64;
        break;
    default:
        return;
    }
    if (p->mode == 8) {
        g_another_stage_id = (s16)D5150[p->count].unk0;
        mode_info.storyWorld = p->count / 10;
        mode_info.storyStage = p->count % 10;
    } else {
        init_course();
        g_another_stage_id = fn_1_A3C84(modeCtrl.difficulty, p->count, modeCtrl.courseFlags);
    }
    p->count++;
    if ((u32)p->count >= limit) {
        p->next = p->mode + 1;
    }
    load_stage(g_another_stage_id);
    event_init(9);
    event_init(0);
    event_init(1);
    event_init(2);
    event_init(3);
    event_init(4);
    event_init(5);
    event_init(7);
    event_init(0xF);
    event_init(0x10);
    event_init(0x11);
    event_init(0x13);
    event_init(0xE);
    event_init(0x15);
    event_init(0x14);
    event_init(9);
    enable_camera(0);
    size = fn_1_664EC();
    if (p->minStage < 0 || p->minSize > size) {
        p->minStage = current_stage_id;
        p->minSize = size;
    }
    if (p->maxStage < 0 || p->maxSize < size) {
        p->maxStage = current_stage_id;
        p->maxSize = size;
    }
    event_dest(9);
}

static inline void dip_switch_test_draw(void)
{
    int i;
    int j;
    int bit;
    char *state;

    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("DIP SWITCH");
    debug_textdraw_set_pos(1, 1);
    debug_textdraw_put_buf_1("\203f\203B\203b\203v\203X\203C\203b\203`");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 16; j++) {
            bit = j + (i * 32) / 2;
            debug_textdraw_set_pos(i * 26 + 2, 6 + j + j / 4);
            if (bit == b4) {
                fn_1_3E100(-1, 0);
                debug_textdraw_set_color(0xC3);
                debug_textdraw_put_buf_1("\034");
                debug_textdraw_set_color(0xFF);
            }
            if (bit == b4) {
                debug_textdraw_set_color(0x33);
            }
            if ((u32)(dip_switches & (1 << bit)) != 0) {
                state = " ON";
            } else {
                state = "OFF";
            }
            debug_textdraw_vsprintf_buf_1("%-18.18s  %s", lbl_1_data_2D964[bit], state);
            if (bit == b4) {
                debug_textdraw_set_color(0xFF);
            }
        }
    }
    debug_textdraw_set_pos((u32)(0x35 - strlen(lbl_1_data_2DD58[b4])) >> 1, 0x1E);
    debug_textdraw_vsprintf_buf_1(lbl_1_data_2DD58[b4]);
}
static inline void romfont_fill_row(u16 *buf, int row) {
    int j;

    for (j = 0; j < 16; j++) {
        buf[j] = j + 0x8140 + (row * 16) + (b5E8 * 256);
    }
    buf[j] = 0;
}

void fn_8_F6B8(void)
{
    char sp8[0x40];
    struct PreRender *pr;
    int i;
    u32 code;

    switch (sub_mode) {
    case 0x73:
        fn_8_770();
        return;
    case 0x77:
        dip_switch_test_draw();
        return;
    case 0x79:
        fn_8_2AB4();
        return;
    case 0x9A:
        fn_8_171C4();
        return;
    case 0x7B:
        blur_test_draw();
        return;
    case 0x7D:
        shadow_test_draw();
        return;
    case 0x7F:
        fn_8_12534();
        return;
    case 0x81:
        fn_8_130A4();
        return;
    case 0x83:
        debug_textdraw_set_pos(4, 4);
        debug_textdraw_vsprintf_buf_1("Category      :%s\n", D1230[s_nl.set].dir);
        if (s_nl.obj == 0) {
            debug_textdraw_vsprintf_buf_1("no data\n");
            return;
        }
        debug_textdraw_vsprintf_buf_1("name          :%s\n", ((char **)s_nl.obj->models[s_nl.model])[-2]);
        debug_textdraw_vsprintf_buf_1("size          :%d\n", *((int **)s_nl.obj->models[s_nl.model])[-1]);
        mtxa_from_mtxb();
        nl2ngc_draw_model_sort_translucent(s_nl.obj->models[s_nl.model]);
        handle_test_camera();
        return;
    case 0x85:
        fn_8_4D5C();
        return;
    case 0x87:
        fn_8_5740();
        return;
    case 0x9E:
        fn_8_5EF0();
        return;
    case 0x89:
        game_main_draw_func();
        return;
    case 0x8B:
        fn_8_16E24();
        mtxa_from_mtxb();
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        avdisp_draw_model_unculled_sort_auto(init_common_gma->modelEntries[0x20].model);
        return;
    case 0x8E:
        fn_8_7A0C();
        return;
    case 0x90:
        fn_8_9C44();
        return;
    case 0x92:
        fn_8_B3C4();
        return;
    case 0x94:
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_put_buf_1("ROMFONT TEST\n");
        debug_textdraw_vsprintf_buf_1("code: %04X\n", (b5E8 << 8) + 0x8140);
        if (lbl_1_bss_D2B7C != 0) {
            fn_1_F0AEC(2, 0x280, 0x1C0);
            for (i = 0; i < 16; i++) {
                code = (b5E8 << 8) + 0x8140;
                if ((code >= 0x8100 && code <= 0x9F00) || (code >= 0xE000 && code <= 0xFC00)) {
                    romfont_fill_row((u16 *)sp8, i);
                    g_some_printf_function_6(0x80, (s16)((i + 1) * 0x18 + 0x20), 0xA, sp8);
                }
            }
        }
        break;
    case 0x96:
        fn_8_C314();
        return;
    case 0x98:
        debug_textdraw_set_pos(1, 1);
        debug_textdraw_put_buf_1("PREVIEW TEST");
        pr = D54E8;
        for (i = 0; (u32)i < 8; i++, pr++) {
            if (i == modeCtrl.unk10) {
                debug_textdraw_set_pos(2, i + 3);
                debug_textdraw_set_color(0x33);
                debug_textdraw_put_buf_1("\034");
                debug_textdraw_set_color(0xFF);
            }
            if ((u32)(modeCtrl.courseFlags & 4) == 0) {
                debug_textdraw_set_pos(3, i + 3);
                debug_textdraw_vsprintf_buf_1("[ %3dx%3d ] %s", pr->w, pr->h, pr->en);
            } else if (i == modeCtrl.unk10) {
                debug_textdraw_set_pos(3, i + 3);
                debug_textdraw_vsprintf_buf_1("[ %3dx%3d ] %s", pr->w, pr->h, pr->en);
                break;
            }
        }
        if ((u32)(modeCtrl.courseFlags & 4) != 0) {
            pr = &D54E8[modeCtrl.unk10];
            debug_textdraw_vsprintf_buf_1(" [ %3d / %3d ]", modeCtrl.unk44, pr->unk10);
        }
        break;
    case 0x9C:
        fn_8_1783C();
        return;
    case 0xA0:
        fn_8_E100();
        return;
    case 0xA2:
        fn_8_EB10();
        return;
    case 0xA4:
        fn_8_EE5C();
        return;
    case 0xA6:
        game_main_draw_func();
        break;
    }
}

