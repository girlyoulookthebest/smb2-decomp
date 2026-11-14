/* dip.c: debug DIP switches and the debug mode toggle. */
#include "types.h"
#include "input.h"
#include "dip.h"
#include "event.h"

u32 dip_switches;
u32 g_some_other_flags;
s16 lbl_1_bss_6FB98;  /* pad that turned on debug mode */
u32 lbl_1_bss_6FB9C;

void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_put_buf_1(char *str);
void event_init(int id);
void event_dest(int id);

/* DIP switch names, by bit. */
char *lbl_1_data_2D964[] =
{
    "DIP_DEBUG",
    "DIP_DISP",
    "DIP_STCOLI",
    "DIP_TRIANGLE",
    "DIP_TAIKEN",
    "DIP_TIME_STOP",
    "DIP_NAMEENTRY",
    "DIP_FIX_WORLD",
    "DIP_TEST_CAM",
    "DIP_NO_INTR",
    "DIP_CAPTURE",
    "DIP_PERF_ALWAYS",
    "DIP_PLAY_STG_ALL",
    "DIP_PLAY_PNT_x10",
    "DIP_SARU_0",
    "DIP_SWITCH15",
    "DIP_ONLY24MB",
    "DIP_SHADOW_DISP",
    "DIP_SPRITE_OFF",
    "DIP_SNDREQ_DISP",
    "DIP_SE_VOL_RESET",
    "DIP_APE_FACEDIR",
    "DIP_SEL_ALL_CTRL",
    "DIP_APE_NUMBER",
    "DIP_1P_TEST_CAM",
    "DIP_PREVIEW_SAVE",
    "DIP_AUTHOR_DEBUG",
    "DIP_SWITCH27",
    "DIP_SOA",
    "DIP_TEST_TEAM",
    "DIP_NO_MINIMAP",
    "DIP_NO_STAGE",
};

/* Descriptions shown in the DIP switch menu. */
char *lbl_1_data_2DD58[] =
{
    "デバッグを有効にする",  /* DIP_DEBUG */
    "デバッグ表示を行う",  /* DIP_DISP */
    "ステージコリジョンを表示",  /* DIP_STCOLI */
    "三角形ステージコリジョンのテスト",  /* DIP_TRIANGLE */
    "体験版モード",  /* DIP_TAIKEN */
    "時間が減らなくなります",  /* DIP_TIME_STOP */
    "ランク外であってもネームエントリーに行く",  /* DIP_NAMEENTRY */
    "ステージを傾けない",  /* DIP_FIX_WORLD */
    "テストカメラ作動",  /* DIP_TEST_CAM */
    "注視点を非表示",  /* DIP_NO_INTR */
    "キャプチャー関連（よく知りません ｂｙ 遠藤）",  /* DIP_CAPTURE */
    "ｚでパフォーマンスメーターをオン・オフ",  /* DIP_PERF_ALWAYS */
    "全てのステージが遊べる",  /* DIP_PLAY_STG_ALL */
    "得られるプレイポイントが１０倍に",  /* DIP_PLAY_PNT_x10 */
    "残りサルが０の状態でゲーム開始",  /* DIP_SARU_0 */
    "未定義",  /* DIP_SWITCH15 */
    "ＲＡＭを２４ＭＢしか使いません",  /* DIP_ONLY24MB */
    "テクスチャ投影による影のデバッグ表示",  /* DIP_SHADOW_DISP */
    "スプライト表示なし",  /* DIP_SPRITE_OFF */
    "ＳＥリクエスト情報をシリアルに表示",  /* DIP_SNDREQ_DISP */
    "ＳＥ基準音量のオフセット計算しないように",  /* DIP_SE_VOL_RESET */
    "猿の熱い視線を映しだす",  /* DIP_APE_FACEDIR */
    "セレクタでコントローラが４本ささってることに",  /* DIP_SEL_ALL_CTRL */
    "サル番号表示",  /* DIP_APE_NUMBER */
    "全てのビューが１Ｐのテストカメラになる",  /* DIP_1P_TEST_CAM */
    "プレビューを保存するモード",  /* DIP_PREVIEW_SAVE */
    "オーサリング開発用デバッグ表示を行なう",  /* DIP_AUTHOR_DEBUG */
    "未定義",  /* DIP_SWITCH27 */
    "SOAデバッグ用",  /* DIP_SOA */
    "テストチームデバッグ用",  /* DIP_TEST_TEAM */
    "ミニマップ表示なし",  /* DIP_NO_MINIMAP */
    "ステージ表示なし",  /* DIP_NO_STAGE */
};

void unset_all_dip_switches(void)
{
    dip_switches = 0;
}

/* Debug mode: with DIP_DEBUG set, Z + Start on any pad toggles it (flags 1 and 2);
 * Z or Z + Right on that pad (or pad 1) toggles the debug display (flag 2). */
void g_check_debug_mode_input(void)
{
    s16 i;
    u32 prevFlags;

    if ((dip_switches & DIP_DEBUG) && (dip_switches & DIP_DISP))
        g_some_other_flags |= 4;
    else
        g_some_other_flags &= ~4;

    if (dip_switches & DIP_DEBUG)
    {
        prevFlags = g_some_other_flags;
        for (i = 0; i < 4; i++)
        {
            if (pad_status_groups[i].unk0[0].err != 0)
                continue;
            if (!(g_some_other_flags & 1))
            {
                if ((analog_inputs[i][0] & 0x200) && (pad_status_groups[i].unk0[2].button & 0x1000))
                {
                    g_some_other_flags |= 3;
                    lbl_1_bss_6FB98 = i;
                }
            }
            else if (i == lbl_1_bss_6FB98 || i == 0)
            {
                if (!(analog_inputs[i][0] & 0x100) && (pad_status_groups[i].unk0[2].button & 0x1000))
                    g_some_other_flags &= ~3;
                else if ((pad_status_groups[i].unk0[2].button & PAD_TRIGGER_Z)
                      || ((pad_status_groups[i].unk0[0].button & PAD_TRIGGER_Z)
                          && (pad_status_groups[i].unk0[4].button & PAD_BUTTON_RIGHT)))
                    g_some_other_flags &= ~2;
                else
                    g_some_other_flags |= 2;
            }
            if (g_some_other_flags != prevFlags)
                break;
        }
    }
    else
    {
        g_some_other_flags &= ~3;
    }

    if (g_some_other_flags & 1)
        lbl_1_bss_6FB9C = 8;
    else if (lbl_1_bss_6FB9C != 0)
        lbl_1_bss_6FB9C--;

    if (g_some_other_flags & 1)
    {
        debug_textdraw_set_pos(43, 0);
        debug_textdraw_put_buf_1("DEBUG MODE");
    }

    if (dip_switches & DIP_DEBUG)
    {
        if ((pad_status_groups[0].unk0[0].button & PAD_BUTTON_B)
         && (pad_status_groups[0].unk0[2].button & PAD_BUTTON_X))
        {
            if (events[18].state != EV_STATE_RUNNING)
            {
                if (g_some_other_flags & 0xA)
                    event_init(18);
            }
            else
            {
                event_dest(18);
            }
        }
    }
    else if (events[18].state != EV_STATE_INACTIVE)
    {
        event_dest(18);
    }
}
