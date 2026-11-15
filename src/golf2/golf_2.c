/* Monkey Golf 2: the golfer monkey's poses and animations (address, swing, putt, reactions),
   and the hole's flag/cup item groups (.text 0x1A868-0x1CF04). */
#include "golf.h"

void call_SoundReqID_arg_0(int soundId);
void call_SoundReqID_arg_1(int id);
void fn_1_113190(struct Ape *ape, int a, float frame, int b, int c);
void fn_1_113600(struct Ape *ape, float frame);
void fn_1_11360C(struct Ape *ape, float x);
void fn_1_1132D0(struct Ape *ape, int b, int c, int d);
int fn_1_11465C(struct Ape *ape, int a, int b);
int fn_1_1146A4(struct Ape *ape, int a, int b);
void fn_1_D250C(float a);
void g_math_unk14(s16 *a, s16 *b, s16 *c);
void g_rumble_controller(s32 pad, s32 a, s32 b);
extern s32 active_monkey_id[];
extern u32 g_player_id_for_sound;
extern u32 g_smth_with_sound;

#define CURR_BALL (balls[modeCtrl.currPlayer])

/* Unreferenced table (two byte lists and 8-byte records). */
struct GolfUnkC68Rec
{
    u8 unk0;
    u8 unk1;
    u16 unk2;
    s16 unk4;
    s16 unk6;
};

struct GolfUnkC68
{
    u8 list1[20];
    u8 list2[20];
    struct GolfUnkC68Rec recs[41];
};

struct GolfUnkC68 lbl_11_data_C68 = {
    {0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D, 0x0E, 0x0F, 0xFF, 0x00},
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00},
    {
        {0x31, 0x00, 0x0000, 0x0000, 0x0000},
        {0x38, 0x00, 0xB400, 0x0001, 0x0000},
        {0x01, 0x00, 0x0000, 0x0002, 0x0000},
        {0x10, 0x00, 0x4C00, 0x0003, 0x0000},
        {0x01, 0x00, 0x0000, 0x0004, 0x0000},
        {0x18, 0x00, 0x0000, 0x0009, 0x0000},
        {0x40, 0x00, 0x0000, 0x0000, 0x0000},
        {0x01, 0x00, 0x0000, 0x000B, 0x0000},
        {0x18, 0x00, 0x0000, 0x000C, 0x0000},
        {0x18, 0x00, 0x0000, 0x0005, 0x0000},
        {0x40, 0x00, 0x0000, 0x0000, 0x0000},
        {0x01, 0x00, 0x0000, 0x0007, 0x0000},
        {0x18, 0x00, 0x0000, 0x0008, 0x0000},
        {0x38, 0x00, 0xB400, 0x000D, 0x0000},
        {0x40, 0x00, 0x0000, 0x0000, 0x0000},
        {0x18, 0x00, 0x0000, 0x0014, 0x0000},
        {0x40, 0x00, 0x0000, 0x0000, 0x0000},
        {0x01, 0x00, 0x0000, 0x0016, 0x0000},
        {0x18, 0x00, 0x0000, 0x0017, 0x0000},
        {0x18, 0x00, 0x0000, 0x0018, 0x0000},
        {0x18, 0x00, 0x0000, 0x000F, 0x0000},
        {0x40, 0x00, 0x0000, 0x0000, 0x0000},
        {0x01, 0x00, 0x0000, 0x0011, 0x0000},
        {0x18, 0x00, 0x0000, 0x0012, 0x0000},
        {0x18, 0x00, 0x0000, 0x0013, 0x0000},
        {0x18, 0x00, 0x0000, 0x0000, 0x0000},
        {0x18, 0x00, 0x0000, 0x0001, 0x0000},
        {0x18, 0x00, 0x0000, 0x0002, 0x0000},
        {0x18, 0x00, 0x0000, 0x0003, 0x0000},
        {0x18, 0x00, 0x0000, 0x0004, 0x0000},
        {0x18, 0x00, 0x0000, 0x0005, 0x0000},
        {0x18, 0x00, 0x0000, 0x0006, 0x0000},
        {0x18, 0x00, 0x0000, 0x0007, 0x0000},
        {0x18, 0x00, 0x0000, 0x0008, 0x0000},
        {0x18, 0x00, 0x0000, 0x0009, 0x0000},
        {0x18, 0x00, 0x0000, 0x000A, 0x0000},
        {0x18, 0x00, 0x0000, 0x000B, 0x0000},
        {0x18, 0x00, 0x0000, 0x000C, 0x0000},
        {0x18, 0x00, 0x0000, 0x000D, 0x0000},
        {0x18, 0x00, 0x0000, 0x000E, 0x0000},
        {0x18, 0x00, 0x0000, 0x000F, 0x0000},
    },
};

/* Which pose function ran last (each resets the frame counter when this changes). */
static u8 lbl_11_data_DD8 = 0xE7;
/* Frame counter of the current pose. */
static u32 lbl_11_data_DDC = 999;

/* Swing animation ids per club (indexed by lbl_11_bss_4C): strong, medium, weak shot. */
struct GolfSwingAnims
{
    s32 strong;
    s32 medium;
    s32 weak;
};

static struct GolfSwingAnims lbl_11_data_DE0[15] = {
    {5, 4, 3}, {5, 4, 3}, {5, 4, 3}, {4, 3, 2}, {4, 3, 2},
    {4, 3, 2}, {3, 2, 1}, {3, 2, 1}, {3, 2, 1}, {3, 2, 1},
    {2, 1, 0}, {2, 1, 0}, {2, 1, 0}, {2, 1, 0}, {2, 1, 0},
};
/* Length (frames) of each swing animation. */
static u32 lbl_11_data_E94[7] = {36, 39, 42, 45, 48, 51, 70};
/* Frame of each swing animation where the club hits the ball. */
static u32 lbl_11_data_EB0[7] = {25, 28, 30, 33, 35, 38, 48};


/* Flag and cup item groups of the hole (separate statics: the code addresses them off
   one .bss anchor). */
static s16 lbl_11_bss_2A80;  /* flag group heading (1/64 turns) */
static s16 lbl_11_bss_2A82;  /* cup group heading (1/64 turns) */
static u8 lbl_11_bss_2A84;   /* flag item group index */
static u8 lbl_11_bss_2A85;   /* cup item group index */
static Mtx lbl_11_bss_2A88;  /* cup group frame -> flag group frame */

/* Address pose at the tee: monkey stands beside the ball. */
void fn_11_1A868(void)
{
    Vec pos;

    if (CURR_BALL.ape == NULL)
        return;
    if (lbl_11_data_DD8 != 0)
    {
        lbl_11_data_DD8 = 0;
        lbl_11_data_DDC = 0;
        CURR_BALL.ape->flags &= ~0x2000;
        CURR_BALL.ape->flags &= ~0x1000;
    }
    switch (active_monkey_id[modeCtrl.currPlayer])
    {
    case 0:
    case 1:
        {
            Vec v = {0.4f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    case 2:
        {
            Vec v = {0.3f, -0.1f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    case 3:
        {
            Vec v = {0.9f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    }
    mtxa_from_translate(&pos);
    mtxa_rotate_z(-0x8000);
    mtxa_rotate_y(-0x4000);
    mtxa_rotate_x(-0x4000);
    mtxa_to_mtx(lbl_11_bss_2A50);
    fn_11_F27C(lbl_11_bss_2A50);
    fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 0, 0);
}

/* Waggle before the shot; returns 1 when done. */
u8 fn_11_1AA9C(void)
{
    Vec pos;

    if (CURR_BALL.ape == NULL)
        return 0;
    if (lbl_11_data_DD8 != 1)
    {
        lbl_11_data_DD8 = 1;
        lbl_11_data_DDC = 0;
        CURR_BALL.ape->flags &= ~0x2000;
        CURR_BALL.ape->flags &= ~0x1000;
    }
    switch (active_monkey_id[modeCtrl.currPlayer])
    {
    case 0:
    case 1:
        {
            Vec v = {0.4f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    case 2:
        {
            Vec v = {0.3f, -0.1f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    case 3:
        {
            Vec v = {0.9f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        break;
    }
    if (lbl_11_data_DDC <= 30)
    {
        if (lbl_11_bss_4C <= 11)
        {
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 3, 0);
            mtxa_from_translate(&pos);
            mtxa_rotate_z(-0x8000);
            mtxa_rotate_y(-0x238E);
            mtxa_rotate_x(-0x471C);
        }
        else
        {
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 0, 1);
            mtxa_from_translate(&pos);
            if (lbl_11_data_DDC <= 16)
            {
                mtxa_rotate_z(32768.0f * ((16 - lbl_11_data_DDC) / 16.0f) + 12743.0f * (lbl_11_data_DDC / 16.0f));
                mtxa_rotate_y(-16384.0f * ((16 - lbl_11_data_DDC) / 16.0f) + -8192.0f * (lbl_11_data_DDC / 16.0f));
                mtxa_rotate_x(-0x4000);
            }
            else
            {
                mtxa_rotate_z(12743.0f * ((33 - lbl_11_data_DDC) / 17.0f));
                mtxa_rotate_y(-8192.0f * ((33 - lbl_11_data_DDC) / 17.0f));
                mtxa_rotate_x(-0x4000);
            }
        }
        mtxa_to_mtx(lbl_11_bss_2A50);
        fn_11_F27C(lbl_11_bss_2A50);
        lbl_11_data_DDC++;
        return 0;
    }
    if (lbl_11_bss_4C <= 11)
    {
        fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 3, 1);
        mtxa_from_translate(&pos);
        mtxa_rotate_z(-0x8000);
        mtxa_rotate_y(-0x238E);
        mtxa_rotate_x(-0x471C);
    }
    else
    {
        fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 0, 2);
        mtxa_from_translate(&pos);
        mtxa_rotate_x(-0x4000);
    }
    mtxa_to_mtx(lbl_11_bss_2A50);
    fn_11_F27C(lbl_11_bss_2A50);
    CURR_BALL.ape->flags &= ~0x2000;
    CURR_BALL.ape->flags &= ~0x1000;
    return 1;
}

/* Backswing; returns 1 when done. */
u8 fn_11_1B040(void)
{
    Vec pos;
    S16Vec rot;

    if (CURR_BALL.ape == NULL)
        return 0;
    if (lbl_11_data_DD8 != 2)
    {
        lbl_11_data_DD8 = 2;
        lbl_11_data_DDC = 0;
        CURR_BALL.ape->flags &= ~0x2000;
        CURR_BALL.ape->flags &= ~0x1000;
    }
    switch (active_monkey_id[modeCtrl.currPlayer])
    {
    case 0:
    case 1:
        {
            Vec v = {0.4f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {-0x471C, -0x238E, -0x8000};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    case 2:
        {
            Vec v = {0.3f, -0.1f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {0x0000, -0x38E3, 0x38E3};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    case 3:
        {
            Vec v = {0.9f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {0x1C71, -0x5555, 0x1C71};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    }
    if (lbl_11_data_DDC <= 35)
    {
        if (lbl_11_bss_4C <= 11)
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 3, 2);
        else
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 0, 3);
        mtxa_from_translate(&pos);
        mtxa_rotate_z(rot.z);
        mtxa_rotate_y(rot.y);
        mtxa_rotate_x(rot.x);
        mtxa_to_mtx(lbl_11_bss_2A50);
        fn_11_F27C(lbl_11_bss_2A50);
        lbl_11_data_DDC++;
        return 0;
    }
    fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 0, 4);
    mtxa_from_translate(&pos);
    mtxa_rotate_z(rot.z);
    mtxa_rotate_y(rot.y);
    mtxa_rotate_x(rot.x);
    mtxa_to_mtx(lbl_11_bss_2A50);
    fn_11_F27C(lbl_11_bss_2A50);
    CURR_BALL.ape->flags &= ~0x2000;
    CURR_BALL.ape->flags &= ~0x1000;
    return 1;
}

/* Downswing and follow-through for a shot of the given strength (0..1); returns 1 when
   done. On a perfect full-power drive the impact freezes for a short slow-motion replay. */
u8 fn_11_1B42C(float *power)
{
    Vec pos;
    S16Vec rot;
    u8 perfect;

    if (CURR_BALL.ape == NULL)
        return 0;
    if (lbl_11_bss_5C == 0.0 && lbl_11_bss_D4 == 1.0)
        perfect = TRUE;
    else
        perfect = FALSE;
    if (lbl_11_data_DD8 != 3)
    {
        lbl_11_data_DD8 = 3;
        lbl_11_data_DDC = 0;
        lbl_11_bss_B40 = 0;
        lbl_11_bss_84 = 0;
        CURR_BALL.ape->flags &= ~0x2000;
        CURR_BALL.ape->flags &= ~0x1000;
    }
    switch (active_monkey_id[modeCtrl.currPlayer])
    {
    case 0:
    case 1:
        {
            Vec v = {0.4f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {-0x471C, -0x238E, -0x8000};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    case 2:
        {
            Vec v = {0.3f, -0.1f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {0x0000, -0x38E3, 0x38E3};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    case 3:
        {
            Vec v = {0.9f, 0.2f, -0.2f};
            Vec *p = &v;

            pos = *p;
        }
        {
            S16Vec r = {0x1C71, -0x5555, 0x1C71};
            S16Vec *q = &r;

            rot = *q;
        }
        break;
    }
    mtxa_from_translate(&pos);
    mtxa_rotate_z(rot.z);
    mtxa_rotate_y(rot.y);
    mtxa_rotate_x(rot.x);
    mtxa_to_mtx(lbl_11_bss_2A50);
    fn_11_F27C(lbl_11_bss_2A50);

    if (0.0 <= *power && *power < 0.333333)
    {
        int anim = lbl_11_data_DE0[lbl_11_bss_4C].weak;

        if (lbl_11_data_DDC > 60 && lbl_11_data_DDC <= lbl_11_data_E94[anim] + 60)
        {
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 1, anim);
            lbl_11_data_DDC++;
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 60)
            {
                if (lbl_11_bss_4C <= 2)
                    call_SoundReqID_arg_0(0xBF);
                else if (lbl_11_bss_4C <= 11)
                    call_SoundReqID_arg_0(0xCC);
                else
                    call_SoundReqID_arg_0(0xCD);
                g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, 0.2 * lbl_80198B38);
            }
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 62)
                call_SoundReqID_arg_1(0x1F);
            return 0;
        }
        if (lbl_11_data_E94[anim] + 60 < lbl_11_data_DDC)
        {
            CURR_BALL.ape->flags &= ~0x1000;
            CURR_BALL.ape->flags &= ~0x2000;
            return 1;
        }
        lbl_11_data_DDC++;
        return 0;
    }
    if (0.3333333 <= *power && *power < 0.66666666)
    {
        int anim = lbl_11_data_DE0[lbl_11_bss_4C].medium;

        if (lbl_11_data_DDC > 60 && lbl_11_data_DDC <= lbl_11_data_E94[anim] + 60)
        {
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 1, anim);
            lbl_11_data_DDC++;
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 60)
            {
                if (lbl_11_bss_4C <= 2)
                    call_SoundReqID_arg_0(0xBF);
                else if (lbl_11_bss_4C <= 11)
                    call_SoundReqID_arg_0(0xCC);
                else
                    call_SoundReqID_arg_0(0xCD);
                g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, 0.3 * lbl_80198B38);
            }
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 62)
                call_SoundReqID_arg_1(0x1F);
            return 0;
        }
        if (lbl_11_data_E94[anim] + 60 < lbl_11_data_DDC)
        {
            CURR_BALL.ape->flags &= ~0x1000;
            CURR_BALL.ape->flags &= ~0x2000;
            return 1;
        }
        lbl_11_data_DDC++;
        return 0;
    }
    if (0.66666666 <= *power)
    {
        int anim = lbl_11_data_DE0[lbl_11_bss_4C].strong;

        if (lbl_11_bss_4C == 0 && perfect)
            anim = 6;
        if (lbl_11_data_DDC > 60 && lbl_11_data_DDC <= lbl_11_data_E94[anim] + 60)
        {
            fn_1_113190(CURR_BALL.ape, 0xF, 0.0f, 1, anim);
            lbl_11_data_DDC++;
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 60)
            {
                g_player_id_for_sound = modeCtrl.currPlayer;
                g_smth_with_sound = active_monkey_id[g_player_id_for_sound];
                if (lbl_11_bss_4C <= 2)
                    call_SoundReqID_arg_0(0xBF);
                else if (lbl_11_bss_4C <= 11)
                    call_SoundReqID_arg_0(0xCC);
                else
                    call_SoundReqID_arg_0(0xCD);
                g_rumble_controller(player_pad_map[modeCtrl.currPlayer], 1, 0.4 * lbl_80198B38);
            }
            if (lbl_11_data_DDC == lbl_11_data_E94[anim] + 60 && perfect && lbl_11_bss_B40 < 2)
            {
                if (lbl_11_bss_84 == 0)
                {
                    lbl_11_bss_84++;
                    lbl_11_data_DDC--;
                    fn_1_D250C(0.0f);
                    lbl_11_bss_F14 = g_font_char_var_3;
                    fn_11_129FC();
                    CURR_BALL.flags |= 0x10;
                    CURR_BALL.ape->flags |= 0x20;
                }
                else if (lbl_11_bss_84 == 15)
                {
                    lbl_11_bss_B40++;
                    lbl_11_bss_84 = 0;
                    lbl_11_data_DDC = 60;
                    CURR_BALL.ape->unk24C = 0;
                    fn_1_113600(CURR_BALL.ape, 0.0f);
                    fn_1_D250C(1.0f);
                    CURR_BALL.flags &= ~0x10;
                    CURR_BALL.ape->flags &= ~0x20;
                }
                else
                {
                    lbl_11_data_DDC--;
                    lbl_11_bss_84++;
                    fn_11_129FC();
                    CURR_BALL.flags |= 0x10;
                    CURR_BALL.ape->flags |= 0x20;
                }
            }
            if (lbl_11_data_DDC == lbl_11_data_EB0[anim] + 62)
                call_SoundReqID_arg_1(0x20);
            return 0;
        }
        if (lbl_11_data_E94[anim] + 60 < lbl_11_data_DDC)
        {
            CURR_BALL.ape->flags &= ~0x1000;
            CURR_BALL.ape->flags &= ~0x2000;
            return 1;
        }
        lbl_11_data_DDC++;
        return 0;
    }
    return 0;
}

void fn_11_1C78C(struct Ape *ape, float frame);

/* Waiting pose while the ball rolls (or idling before the shot). */
void fn_11_1C08C(void)
{
    if (CURR_BALL.ape == NULL)
        return;
    if (lbl_11_data_DD8 != 4)
    {
        lbl_11_data_DD8 = 4;
        lbl_11_data_DDC = 0;
    }
    if (fn_11_EC78() || !fn_11_EC00(modeCtrl.currPlayer, fn_11_EC30()))
    {
        fn_1_113190(CURR_BALL.ape, 0, 0.0f, 0, 0);
        fn_1_113768(CURR_BALL.ape, 0.0f);
    }
    else if (CURR_BALL.unk154 == 0)
        fn_11_1C78C(CURR_BALL.ape, 0.0f);
    else if (fn_11_EB28())
        fn_1_113190(CURR_BALL.ape, 0, CURR_BALL.ape->unk27C++, 1, 0);
    else
        fn_11_1C78C(CURR_BALL.ape, mathutil_vec_len(&CURR_BALL.vel));
}

/* Like fn_11_1C08C, but plays a one-second reaction (anim 6) first. */
void fn_11_1C2FC(void)
{
    if (CURR_BALL.ape == NULL)
        return;
    if (lbl_11_data_DD8 != 5)
    {
        lbl_11_data_DD8 = 5;
        lbl_11_data_DDC = 0;
    }
    lbl_11_data_DDC++;
    if (lbl_11_data_DDC < 60)
        fn_1_113190(CURR_BALL.ape, 0, 0.0f, 6, 0);
    else if (CURR_BALL.unk154 == 0)
        fn_11_1C78C(CURR_BALL.ape, 0.0f);
    else if (fn_11_EB28())
        fn_1_113190(CURR_BALL.ape, 0, CURR_BALL.ape->unk27C++, 1, 0);
    else
        fn_11_1C78C(CURR_BALL.ape, mathutil_vec_len(&CURR_BALL.vel));
}

/* Reaction to the hole result (score relative to par). */
void fn_11_1C538(int result)
{
    struct Ape *ape;

    if (CURR_BALL.ape == NULL)
        return;
    ape = CURR_BALL.ape;
    if (lbl_11_data_DD8 != 6)
    {
        lbl_11_data_DD8 = 6;
        lbl_11_data_DDC = 0;
        ape->flags &= ~0x2000;
        ape->flags &= ~0x1000;
    }
    if (ape->flags & 0x4000)
    {
        fn_1_113190(ape, 0, ape->unk27C++, 7, 1);
        return;
    }
    switch (result)
    {
    case -10:
        fn_1_113190(ape, 0, 0.0f, 7, 8);
        break;
    case -3:
        fn_1_113190(ape, 0, 0.0f, 7, 6);
        break;
    case -2:
        fn_1_113190(ape, 0, 0.0f, 7, 4);
        break;
    case -1:
        fn_1_113190(ape, 0, 0.0f, 7, 2);
        break;
    case 0:
        fn_1_113190(ape, 0, 0.0f, 7, 0);
        break;
    case 1:
    default:
        if (fn_1_11465C(ape, 0xF, 2) == 0 || fn_1_1146A4(ape, 0xF, 0) == 0)
        {
            fn_1_113768(ape, 0.0f);
            fn_1_11360C(ape, 20.0f);
        }
        fn_1_1132D0(ape, 0xF, 2, 0);
        break;
    }
}

/* Idle/watching animation; picks the pose from the ball's state and speed. */
void fn_11_1C78C(struct Ape *ape, float frame)
{
    int a;
    int b;

    if (lbl_11_bss_2A48 > 0)
        lbl_11_bss_2A48--;
    if (ape->flags & 2)
    {
        a = 5;
        b = 0;
    }
    else if (ape->flags & 1)
    {
        a = 1;
        b = 0;
        frame = ape->unk27C++;
    }
    else if (CURR_BALL.flags & 2)
    {
        a = 4;
        b = 1;
        if (lbl_11_bss_2A48 == 0)
        {
            lbl_11_bss_2A48 = 30;
            lbl_11_bss_2A4C = (rand() & 0x7FFF) % 8;
            if (lbl_11_bss_2A4C == 0)
                call_SoundReqID_arg_0(0x3A);
            else if (lbl_11_bss_2A4C == 1)
                call_SoundReqID_arg_0(0x3B);
            else if (lbl_11_bss_2A4C == 2)
                call_SoundReqID_arg_0(0x3F);
            else if (lbl_11_bss_2A4C == 3)
                call_SoundReqID_arg_0(0x53);
            else if (lbl_11_bss_2A4C == 4)
                call_SoundReqID_arg_0(0x54);
            else if (lbl_11_bss_2A4C == 5)
                call_SoundReqID_arg_0(0x55);
            else if (lbl_11_bss_2A4C == 6)
                call_SoundReqID_arg_0(0x56);
            else if (lbl_11_bss_2A4C == 7)
                call_SoundReqID_arg_0(0x57);
        }
    }
    else
    {
        float speed = 216.0f * mathutil_vec_len(&CURR_BALL.vel);

        a = 2;
        if (speed > 100.0)
            b = 12;
        else if (speed > 65.0)
            b = 11;
        else if (speed > 30.0)
            b = 10;
        else if (speed > 16.0)
            b = 9;
        else if (speed > 13.0)
            b = 8;
        else if (speed > 10.0)
            b = 7;
        else if (speed > 8.4)
            b = 6;
        else if (speed > 7.0)
            b = 5;
        else if (speed > 5.5)
            b = 4;
        else if (speed > 4.0)
            b = 3;
        else if (speed > 2.5)
            b = 2;
        else if (speed > 1.0)
            b = 1;
        else
            b = 0;
    }
    fn_1_113190(ape, 0, frame, a, b);
}

/* Picks random headings for the flag and cup item groups and the matrix taking the cup
   group's frame to the flag group's (the higher grid step is the flag). */
void fn_11_1CB74(void)
{
    struct StageCollHeader *hdr;
    Mtx mtx;


    lbl_11_bss_2A80 = (rand() & 0x7FFF) % 64;
    lbl_11_bss_2A82 = (rand() & 0x7FFF) % 64;
    if (stagedef->collHeaders[1].gridStepX > stagedef->collHeaders[2].gridStepX)
    {
        lbl_11_bss_2A84 = 1;
        lbl_11_bss_2A85 = 2;
    }
    else
    {
        lbl_11_bss_2A84 = 2;
        lbl_11_bss_2A85 = 1;
    }
    hdr = &stagedef->collHeaders[lbl_11_bss_2A85];
    mtxa_from_translate(&hdr->initPos);
    mtxa_rotate_z(hdr->initRot.z);
    mtxa_rotate_y(hdr->initRot.y);
    mtxa_rotate_x(hdr->initRot.x);
    mtxa_to_mtx(mtx);
    hdr = &stagedef->collHeaders[lbl_11_bss_2A84];
    mtxa_from_identity();
    mtxa_rotate_x(-hdr->initRot.x);
    mtxa_rotate_y(-hdr->initRot.y);
    mtxa_rotate_z(-hdr->initRot.z);
    mtxa_translate_neg(&hdr->initPos);
    mtxa_mult_right(mtx);
    mtxa_to_mtx(lbl_11_bss_2A88);
}

/* Applies the random headings to the two item groups' transforms. */
void fn_11_1CCF0(void)
{
    struct ItemGroup *ig;
    struct StageCollHeader *hdr;

    ig = &itemgroups[lbl_11_bss_2A84];
    hdr = &stagedef->collHeaders[lbl_11_bss_2A84];
    ig->prevRot.y = ig->rot.y = (lbl_11_bss_2A80 << 16) / 64;
    mtxa_from_translate(&ig->pos);
    mtxa_rotate_z(ig->rot.z);
    mtxa_rotate_y(ig->rot.y);
    mtxa_rotate_x(ig->rot.x - hdr->initRot.x);
    mtxa_rotate_y(-hdr->initRot.y);
    mtxa_rotate_z(-hdr->initRot.z);
    mtxa_translate_neg(&hdr->initPos);
    mtxa_to_mtx(ig->transform);
    mtxa_to_mtx(ig->prevTransform);

    mtxa_from_translate(&ig->pos);
    mtxa_rotate_z(ig->rot.z);
    mtxa_rotate_y(ig->rot.y);
    mtxa_rotate_x(ig->rot.x);
    mtxa_mult_right(lbl_11_bss_2A88);
    ig = &itemgroups[lbl_11_bss_2A85];
    hdr = &stagedef->collHeaders[lbl_11_bss_2A85];
    mtxa_rotate_y((lbl_11_bss_2A82 << 16) / 64);
    mathutil_mtxA_get_translate(&ig->pos);
    g_math_unk14(&ig->rot.z, &ig->rot.y, &ig->rot.x);
    ig->prevPos.x = ig->pos.x;
    ig->prevPos.y = ig->pos.y;
    ig->prevPos.z = ig->pos.z;
    ig->prevRot.x = ig->rot.x;
    ig->prevRot.y = ig->rot.y;
    ig->prevRot.z = ig->rot.z;
    mtxa_rotate_x(-hdr->initRot.x);
    mtxa_rotate_y(-hdr->initRot.y);
    mtxa_rotate_z(-hdr->initRot.z);
    mtxa_translate_neg(&hdr->initPos);
    mtxa_to_mtx(ig->transform);
    mtxa_to_mtx(ig->prevTransform);
}
