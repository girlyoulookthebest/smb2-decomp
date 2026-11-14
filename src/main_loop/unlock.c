/* Play points, gifts and unlockables. */
#include "types.h"
#include "libc.h"
#include "course.h"
#include "dip.h"
#include "mode.h"

struct UnlockInfo
{
    /*0x00*/ u8 masterUnlocked;
    /*0x01*/ u8 monkeyCount;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 staffCreditsGame;
    /*0x04*/ int playPoints;
    /*0x08*/ int gamePoints;
    /*0x0C*/ u8 goalCount;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u16 movies;
    /*0x10*/ u32 partyGames;
    /*0x14*/ u16 moviesSeen;
    u8 unk16[0x84 - 0x16];
}; /* size = 0x84 */

struct TextBox
{
    /*0x00*/ int state;
    u8 unk4[0xC - 0x4];
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    u8 unk10[0x14 - 0x10];
    /*0x14*/ u8 unk14;
    /*0x15*/ u8 unk15;
    /*0x16*/ u8 unk16;
    /*0x17*/ s8 unk17;
    /*0x18*/ u8 unk18;
    /*0x1C*/ void (*callback)(struct TextBox *tb);
    u8 unk20[0x2C - 0x20];
    /*0x2C*/ u32 flags;
    u8 unk30[4];
}; /* size = 0x34 */

extern s32 locale;
extern u8 storymode_save_files[];
extern u8 g_some_gift_menu_flags;
extern u16 merged_digital_inputs[];

void call_SoundReqID_arg_0(int id);
void call_SoundReqID_arg_1(int id);
int g_some_printf_function_4(int id, char *fmt, ...);
void g_smth_with_playpoint_or_gift_msg(int id, char *msg);
void g_display_playpoint_or_gift_message_child(int id, int state, struct TextBox *tb);

void fn_1_16C52C(void);
void set_unlocked_monkeys_to_three(void);
int are_all_gifts_unlocked(void);
int are_all_party_games_unlocked(void);
int are_all_extra_monkeys_unlocked(void);
int are_all_movies_unlocked(void);
int is_staff_credits_game_unlocked(void);
int is_able_to_unlock_party_game(void);
int is_able_to_unlock_gift(void);
int fn_1_16CEBC(void);
int fn_1_16CFD4(void);
int fn_1_16D350(int id);
int g_is_movie_unlocked(int id);
int fn_1_16D420(void);
void g_playpoint_or_gift_msg_disp(struct TextBox *tb);

u32 lbl_1_data_7FCA0[] = {
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

int g_playpoint_msg_counter;
s32 lbl_1_bss_DDA3C[4];
u8 lbl_1_bss_DDA38;
struct UnlockInfo unlock_info;

struct UnlockInfo *unlock_info_ptr = &unlock_info;

static s16 storyPoints[] = {
    10, 20, 30, 20, 30, 80, 40, 90, 100, 80,
    20, 50, 80, 80, 70, 80, 50, 80, 100, 90,
    10, 30, 60, 50, 40, 50, 70, 100, 80, 90,
    20, 60, 60, 10, 80, 50, 60, 90, 90, 90,
    60, 70, 40, 60, 30, 50, 40, 80, 100, 80,
    30, 30, 30, 90, 50, 90, 80, 90, 70, 90,
    20, 70, 30, 60, 20, 90, 90, 100, 80, 70,
    80, 60, 80, 80, 60, 50, 90, 100, 80, 80,
    60, 80, 30, 80, 100, 90, 100, 60, 60, 90,
    20, 60, 30, 100, 40, 20, 50, 70, 100, 60,
    10, 50, 150, 200, 400, 500, 600, 700, 800, 900,
};
static s16 lbl_c0[] = {
    10, 20, 10, 20, 10, 30, 20, 80, 60, 50,
};
static s16 lbl_c1[] = {
    20, 50, 30, 60, 10, 30, 70, 60, 80, 20,
    30, 80, 60, 60, 40, 30, 30, 80, 30, 30,
    20, 80, 50, 10, 60, 90, 60, 80, 80, 300,
};
static s16 lbl_c2[] = {
    30, 70, 40, 80, 10, 50, 20, 60, 100, 20,
    80, 80, 50, 50, 50, 90, 90, 50, 90, 30,
    40, 50, 70, 60, 40, 80, 90, 90, 100, 40,
    90, 80, 100, 90, 80, 90, 100, 100, 60, 50,
    100, 100, 80, 90, 100, 70, 80, 80, 60, 500,
};
static s16 lbl_c3[] = {
    60, 70, 60, 30, 30, 40, 20, 50, 70, 100,
};
static s16 lbl_c4[] = {
    130, 190, 130, 120, 180, 200, 190, 190, 180, 400,
};
static s16 lbl_c5[] = {
    220, 220, 190, 240, 240, 230, 240, 230, 250, 600,
};
static s16 lbl_c6[] = {
    220, 260, 280, 280, 260, 270, 290, 280, 260, 700,
};
static s16 lbl_c7[] = {
    360, 340, 400, 400, 390, 390, 390, 400, 390, 800,
};
static s16 *coursePoints[9] = {
    lbl_c0, lbl_c1, lbl_c2, lbl_c3, lbl_c4, lbl_c5, lbl_c6, lbl_c7, lbl_c7,
};
static s16 streak[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
};

static char *msgReceived[6] = {
    "You received %d Play Points.",
    "",
    "",
    "",
    "",
    "",
};
static char *msgNone[6] = {
    "You didn't get any Play Points.",
    "",
    "",
    "",
    "",
    "",
};
static char *msgTotal[6] = {
    "You now have a total of %d Play Points.",
    "",
    "",
    "",
    "",
    "",
};
static char *msgLeft[6] = {
    "Only %d Play Points left\nto unlock the next Party Game!",
    "",
    "",
    "",
    "",
    "",
};
static char *msgCanUnlock[6] = {
    "/bcff8000/You can unlock a Party Game!\nUnlock a Party Game by selecting it!",
    "",
    "",
    "",
    "",
    "",
};
static char *msgEarnMore[6] = {
    "Earn more Play Points\nto get a gift!",
    "",
    "",
    "",
    "",
    "\202\250\202\334\202\257\202\360 \203Q\203b\203g\202\305\202\253\202\351\202\346\202\244\202\311\n\202\340\202\301\202\306 \203v\203\214\203C\203|\203C\203\223\203g\202\360 \202\275\202\337\202\334\202\265\202\345\202\244\201B",
};
static char *msgGift[6] = {
    "/bcff8000/Now you can get a gift!\nCheck the Gifts in the Options Menu!",
    "",
    "",
    "",
    "",
    "/bcff8000/\202\250\202\334\202\257\202\360 \203Q\203b\203g\202\305\202\253\202\334\202\267\201I\n\203I\203v\203V\203\207\203\223\202\311\202\240\202\351 \202\250\202\334\202\257\202\360 \203`\203F\203b\203N\202\265\202\334\202\265\202\345\202\244\201I",
};
static char *msgRecord[6] = {
    "Play Point record for this time:%d",
    "Momentane Spielpunktstatistik:%d",
    "Record de pts jeux pour cette fois:%d",
    "R\351cord puntos juego esta vez:%d",
    "Record punti gioco della partita:%d",
    "\202\261\202\361\202\251\202\242\202\314\203v\203\214\203C\203|\203C\203\223\203g\202\253\202\353\202\255\201F%d",
};
static char *msgBest[6] = {
    "Highest Play Points record:%d",
    "Spielpunktrekord:%d",
    "Meilleur record pts de jeu:%d",
    "R\351c. M\341x. puntos juego:%d",
    "Miglior record punti gioco:%d",
    "\203v\203\214\203C\203|\203C\203\223\203g\202\314\203T\203C\203R\201[\202\253\202\353\202\255\201F%d",
};
static char *msgBeat[6] = {
    "Do your best to try and beat\nthe high score, okay?",
    "",
    "",
    "",
    "",
    "\203T\203C\203R\201[\202\253\202\353\202\255\202\360 \202\261\202\246\202\347\202\352\202\351\202\346\202\244\202\311\n\202\261\202\352\202\251\202\347\202\340\201A\202\252\202\361\202\316\202\350\202\334\202\265\202\345\202\244\201I",
};
static char *msgNew[6] = {
    "That's a /bcff8000/new high score!/bc000000/ Congratulations!\nThat was truly a great game!",
    "",
    "",
    "",
    "",
    "\202\261\202\352\202\315\201A/bcff8000/\203T\203C\203R\201[\202\253\202\353\202\255/bc000000/\202\305\202\267\201I\202\250\202\337\202\305\202\306\202\244\202\262\202\264\202\242\202\334\202\267\201I\n\202\306\202\304\202\340 \202\267\202\316\202\347\202\265\202\242\203v\203\214\203C\202\305\202\265\202\275\201I",
};

void clear_unlock_info(void)
{
    memset(unlock_info_ptr, 0, sizeof(*unlock_info_ptr));
    fn_1_16C52C();
    set_unlocked_monkeys_to_three();
}

void fn_1_16C348(void)
{
    memcpy_handler(storymode_save_files + 0x274, unlock_info_ptr, sizeof(*unlock_info_ptr));
}

void fn_1_16C384(void)
{
    memcpy_handler(unlock_info_ptr, storymode_save_files + 0x274, sizeof(*unlock_info_ptr));
}

void unlock_master(void)
{
    unlock_info_ptr->masterUnlocked = 1;
}

int is_master_unlocked(void)
{
    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_STG_ALL))
        return 1;
    return unlock_info_ptr->masterUnlocked;
}

void fn_1_16C424(int val)
{
    if (val > unlock_info_ptr->unkD)
    {
        if (val > 2)
            unlock_info_ptr->unkD = 2;
        else
            unlock_info_ptr->unkD = val;
    }
}

int fn_1_16C474(void)
{
    if ((dip_switches & 0x1001) == 0x1001)
        return 2;
    return unlock_info_ptr->unkD;
}

void fn_1_16C4A8(int val)
{
    if (val > unlock_info_ptr->unk2)
    {
        if (val > 2)
            unlock_info_ptr->unk2 = 2;
        else
            unlock_info_ptr->unk2 = val;
    }
}

int fn_1_16C4F8(void)
{
    if ((dip_switches & 0x1001) == 0x1001)
        return 2;
    return unlock_info_ptr->unk2;
}

void fn_1_16C52C(void) {}

void g_set_unlockables_status(void)
{
    unlock_info_ptr->gamePoints = 0;
    unlock_info_ptr->goalCount = 0;
}

void fn_1_16C55C(void)
{
    unlock_info_ptr->gamePoints = 0;
    unlock_info_ptr->goalCount = 0;
    unlock_info_ptr->playPoints = 0;
}

void fn_1_16C59C(void)
{
    unlock_info_ptr->gamePoints = 0;
}

void fn_1_16C5B4(void)
{
    unlock_info_ptr->goalCount = 0;
}

void g_handle_goal(void)
{
    int points;

    if (modeCtrl.gameType == 14)
    {
        points = storyPoints[mode_info.storyWorld + mode_info.storyStage * 10];
        points += storyPoints[mode_info.storyWorld + 100] / 10;
    }
    else
    {
        points = coursePoints[calc_course_idx(modeCtrl.difficulty, modeCtrl.courseFlags)][mode_info.currFloor - 1];
    }
    points += streak[unlock_info_ptr->goalCount];
    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_PNT_x10))
        points *= 10;
    unlock_info_ptr->gamePoints += points;
    unlock_info_ptr->goalCount++;
}

void fn_1_16C720(void) {}

void compare_play_points_with_99999_after_exit_game(void)
{
    u32 points = unlock_info_ptr->gamePoints;

    if (are_all_gifts_unlocked())
    {
        if (points > unlock_info_ptr->playPoints)
            unlock_info_ptr->playPoints = points;
    }
    else
    {
        unlock_info_ptr->playPoints = unlock_info_ptr->playPoints + points < 99999
            ? unlock_info_ptr->playPoints + points : 99999;
    }
}

int get_play_point_count(void)
{
    return unlock_info_ptr->playPoints;
}

int fn_1_16C7F8(void)
{
    return unlock_info_ptr->gamePoints;
}

void g_display_playpoint_or_gift_message(int param, float x, float y)
{
    struct TextBox tb;

    memset(&tb, 0, sizeof(tb));
    tb.flags = 4;
    tb.unk16 = 13;
    tb.x = x;
    tb.y = y;
    tb.unk14 = 25;
    tb.unk15 = 2;
    tb.unk17 = (s8)param;
    tb.unk18 = 0;
    tb.callback = g_playpoint_or_gift_msg_disp;
    g_display_playpoint_or_gift_message_child(1, 1, &tb);
    g_playpoint_msg_counter = 480;
}

int fn_1_16C8D8(void)
{
    if (g_playpoint_msg_counter != 0 && g_playpoint_msg_counter < 450)
        return 1;
    return 0;
}

int fn_1_16C910(void)
{
    return g_playpoint_msg_counter != 0;
}

void fn_1_16C92C(void)
{
    if (g_playpoint_msg_counter != 0 && g_playpoint_msg_counter >= 30)
        g_playpoint_msg_counter = 30;
}

void g_playpoint_or_gift_msg_disp(struct TextBox *tb)
{
    if (g_playpoint_msg_counter == 480)
    {
        if (are_all_gifts_unlocked())
        {
            g_some_printf_function_4(1, msgRecord[locale], unlock_info_ptr->gamePoints);
            g_some_printf_function_4(1, msgBest[locale], unlock_info_ptr->playPoints);
        }
        else if (unlock_info_ptr->gamePoints == 0)
        {
            g_some_printf_function_4(1, msgNone[locale]);
            g_some_printf_function_4(1, msgTotal[locale], unlock_info_ptr->playPoints);
        }
        else
        {
            g_some_printf_function_4(1, msgReceived[locale], unlock_info_ptr->gamePoints);
            g_some_printf_function_4(1, msgTotal[locale], unlock_info_ptr->playPoints);
        }
    }
    else if (g_playpoint_msg_counter == 240)
    {
        if (are_all_gifts_unlocked())
        {
            if (unlock_info_ptr->playPoints < unlock_info_ptr->gamePoints)
            {
                g_smth_with_playpoint_or_gift_msg(1, msgNew[locale]);
                call_SoundReqID_arg_1(0x395);
            }
            else
                g_smth_with_playpoint_or_gift_msg(1, msgBeat[locale]);
        }
        else if (are_all_party_games_unlocked())
        {
            if (is_able_to_unlock_gift() || fn_1_16CEBC() || fn_1_16CFD4())
            {
                g_some_printf_function_4(1, msgGift[locale]);
                call_SoundReqID_arg_1(0x395);
            }
            else
                g_some_printf_function_4(1, msgEarnMore[locale]);
        }
        else if (is_able_to_unlock_party_game())
        {
            g_some_printf_function_4(1, msgCanUnlock[locale]);
            call_SoundReqID_arg_1(0x395);
        }
        else
            g_some_printf_function_4(1, msgLeft[locale], 2500 - unlock_info_ptr->playPoints);
    }
    else if (g_playpoint_msg_counter > 240)
    {
        if ((tb->flags & 2) && tb->state == 10 && (merged_digital_inputs[2] & 0x100))
            g_playpoint_msg_counter = 241;
    }
    else if (g_playpoint_msg_counter > 30)
    {
        if ((tb->flags & 2) && tb->state == 10 && (merged_digital_inputs[2] & 0x100))
            g_playpoint_msg_counter = 30;
    }
    if (tb->unk17 == 1)
    {
        if (g_playpoint_msg_counter == 31)
            return;
        if (g_playpoint_msg_counter == 30)
            g_display_playpoint_or_gift_message_child(1, 20, NULL);
    }
    else if (g_playpoint_msg_counter == 30)
        g_display_playpoint_or_gift_message_child(1, 20, NULL);
    if (g_playpoint_msg_counter > 0)
        g_playpoint_msg_counter--;
}

int is_able_to_unlock_party_game(void)
{
    return unlock_info_ptr->playPoints >= 2500;
}

void unlock_party_game(int id)
{
    call_SoundReqID_arg_0(0x16D);
    unlock_info_ptr->playPoints -= 2500;
    unlock_info_ptr->partyGames |= 1 << id;
}

int is_able_to_unlock_gift(void)
{
    return unlock_info_ptr->playPoints >= 500;
}

void g_something_with_assinging_unlockables(void)
{
    call_SoundReqID_arg_0(0x16D);
    unlock_info_ptr->playPoints -= 500;
    unlock_info_ptr->monkeyCount++;
}

int fn_1_16CEBC(void)
{
    return unlock_info_ptr->playPoints >= 1000;
}

int fn_1_16CEE4(void)
{
    int i;

    if (unlock_info_ptr->playPoints < 1000)
        return 0;
    for (i = 0; i < 12; i++)
    {
        if (!fn_1_16D350(i) && g_is_movie_unlocked(i))
            return 1;
    }
    return 0;
}

void unlock_movie(int id)
{
    call_SoundReqID_arg_0(0x16D);
    unlock_info_ptr->playPoints -= 1000;
    unlock_info_ptr->movies |= 1 << id;
}

int fn_1_16CFD4(void)
{
    return unlock_info_ptr->playPoints >= 1000;
}

int fn_1_16CFFC(void)
{
    if (unlock_info_ptr->playPoints < 1000)
        return 0;
    return g_some_gift_menu_flags & 8;
}

void unlock_staff_credits_game(void)
{
    call_SoundReqID_arg_0(0x16D);
    unlock_info_ptr->playPoints -= 1000;
    unlock_info_ptr->staffCreditsGame = 1;
}

int are_all_gifts_unlocked(void)
{
    if (!are_all_party_games_unlocked())
        return 0;
    if (!are_all_extra_monkeys_unlocked())
        return 0;
    if (are_all_movies_unlocked() == 0)
        return 0;
    return is_staff_credits_game_unlocked() != 0;
}

int g_check_if_partygame_unlocked(int id)
{
    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_STG_ALL))
        return 1;
    return unlock_info_ptr->partyGames & (1 << id);
}

int get_num_of_unlocked_party_games(void)
{
    int i;
    int count = 0;

    for (i = 0; i < 32; i++)
    {
        if (g_check_if_partygame_unlocked(i))
            count++;
    }
    return count + 6 < 12 ? count + 6 : 12;
}

int are_all_party_games_unlocked(void)
{
    return get_num_of_unlocked_party_games() == 12;
}

void set_unlocked_monkeys_to_three(void)
{
    unlock_info_ptr->monkeyCount = 3;
}

u8 get_num_unlocked_monkeys(void)
{
    return unlock_info_ptr->monkeyCount;
}

int are_all_extra_monkeys_unlocked(void)
{
    return get_num_unlocked_monkeys() == 99;
}

void g_set_movie_as_unlocked(int id)
{
    unlock_info_ptr->moviesSeen |= 1 << id;
}

int g_is_movie_unlocked(int id)
{
    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_PLAY_STG_ALL))
        return 1;
    return unlock_info_ptr->moviesSeen & (1 << id);
}

int fn_1_16D350(int id)
{
    return unlock_info_ptr->movies & (1 << id);
}

int are_all_movies_unlocked(void)
{
    int count = 0;
    int i;

    for (i = 0; i < 12; i++)
    {
        if (fn_1_16D350(i))
            count++;
    }
    return count == 12;
}

int is_staff_credits_game_unlocked(void)
{
    return unlock_info_ptr->staffCreditsGame;
}

int return_5(void)
{
    return 5;
}

int fn_1_16D3E0(void)
{
    if (fn_1_16D420())
        return 10;
    return 5 - mode_info.continuesUsed;
}

int fn_1_16D420(void)
{
    return 0;
}
