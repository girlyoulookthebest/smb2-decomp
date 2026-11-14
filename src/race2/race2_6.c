#include "background.h"
#include "race2.h"
#include "item.h"
#include "libc.h"
#include "event.h"
#include "input.h"
#include "sprite.h"
#include "sound.h"
#include "mathutil.h"
#include "camera.h"
#include "mode.h"
#include "ball.h"
#include "stage.h"
#include "bitmap.h"
#include "textdraw.h"

struct Race2Marker
{
    Vec unk0;
};

extern s32 lbl_10_bss_1224;
extern s16 lbl_10_bss_121C;
extern s16 lbl_10_bss_2D50[8];
extern s32 lbl_10_bss_418C;
extern struct RacePlayer *lbl_10_bss_4238[8];
extern u32 lbl_1_data_2E490[];
extern s32 locale;
extern s32 race_frames_remaining;
extern s16 player_count;
void textdraw_printf(char *fmt, ...);
void textdraw_put_char(u8 c);
#ifdef __MWERKS__
int strlen(const char *);
#endif

extern char lbl_10_data_2D7C[];
extern char lbl_10_data_2D8C[];
extern char lbl_10_data_2D9C[];
extern char lbl_10_data_2DAC[];
extern char lbl_10_data_2DBC[];
extern char lbl_10_data_2DCC[];
extern char lbl_10_data_2DDC[];
extern char lbl_10_data_2DEC[];

struct Race2Marker lbl_10_bss_4260[8];
float textdraw_get_pixel_height_of_string(const char *s);
float textdraw_get_pixel_width_of_string_as_f32(const char *s);
void textdraw_set_alignment(u8 align);
void textdraw_reset(void);
void textdraw_set_alpha(float alpha);
void textdraw_set_depth(float depth);
void textdraw_set_flags(u32 flags);
void textdraw_set_font(int fontId);
void textdraw_set_mul_color(u32 color);
void textdraw_set_add_color(u32 color);
void textdraw_set_pos(float x, float y);
void textdraw_set_scale(float x, float y);
void textdraw_set_border(void);
void textdraw_set_font_style(int style);
void textdraw_print(const char *s);
void fn_10_21114(void)
{
    char *sp14[8] = {
        lbl_10_data_2D7C, lbl_10_data_2D8C, lbl_10_data_2D9C, lbl_10_data_2DAC,
        lbl_10_data_2DBC, lbl_10_data_2DCC, lbl_10_data_2DDC, lbl_10_data_2DEC,
    };
    s16 r30;
    s16 r29;
    char *tr28;
    s16 tr27;
    struct Race2Marker *tr26;
    char *tr25;
    s16 tr0;
    struct GMAModel *tr4;

    for (r30 = 0; r30 < 8; r30++) {
        tr28 = sp14[r30];
        tr27 = strlen(tr28);
        tr26 = &lbl_10_bss_4260[r30];
        tr26->unk0 = (Vec){ 0.0f, 0.0f, 0.0f };
        lbl_10_bss_148[r30] = 0;
        for (r29 = 0; r29 < (s32) stage_gma->numModels; r29++) {
            tr25 = stage_gma->modelEntries[r29].name;
            tr0 = (s16) strlen(tr25) - tr27;
            if ((tr0 >= 0) && (strcmp(tr28, tr25 + tr0) == 0)) {
                lbl_10_bss_148[r30] = stage_gma->modelEntries[r29].model;
                tr4 = lbl_10_bss_148[r30];
                tr26->unk0 = tr4->boundSphereCenter;
                tr26->unk0.y += 5.0;
                break;
            }
        }
    }
}

