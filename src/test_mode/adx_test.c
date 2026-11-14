#include "types.h"
#include "dip.h"
#include "input.h"
#include "libc.h"
#include "mode.h"

void fn_1_3A040(int a, int b);
void fn_1_3A26C(void);
void fn_1_3A340(int a, int b);
void fn_1_3A45C(int a, int b);
void fn_1_3A538(int a);
void fn_1_3A5C4(int a);
void fn_1_3A6E8(int a, int b);
void fn_1_3A8B4(int a, int b, int c, int d);
void fn_1_3ACD8(int a, int b, int c);
void fn_1_3AEEC(int a, int b, int c, int d);
int fn_1_3B408(int a);
int fn_1_3B514(int a);
int fn_1_3B64C(int a);
int fn_1_3B6EC(int a);
char *fn_1_3B790(int a);
char *fn_1_3B7D0(int a);
int fn_1_3B97C(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(u8 color);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
int strncmp(const char *a, const char *b, int n);

static int fileId;
static int volume;
static int pan;
static int paused;
static char status[7][0x28];

void fn_8_16EEC(void)
{
    fileId = 0;
    volume = 0;
    pan = 0;
    paused = 0;
}

void fn_8_16F18(void)
{
    if ((g_some_other_flags & 0xA) != 0)
        return;
    if (pad_status_groups->unk0[2].button & 0x100)
    {
        fn_1_3A040(2, fileId);
        paused = 0;
    }
    if (pad_status_groups->unk0[2].button & 0x200)
    {
        fn_1_3A26C();
        paused = 0;
    }
    if (pad_status_groups->unk0[2].button & 0x800)
    {
        if (paused != 0)
        {
            fn_1_3A5C4(2);
            paused = 0;
        }
        else
        {
            fn_1_3A538(2);
            paused = 1;
        }
    }
    if (pad_status_groups->unk0[2].button & 0x40)
        fn_1_3A6E8(2, 300);
    if (pad_status_groups->unk0[2].button & 0x20)
        fn_1_3A8B4(2, fileId, volume, 300);
    if (pad_status_groups->unk0[2].button & 0x400)
        fn_1_3ACD8(fileId, volume, 300);
    if (pad_status_groups->unk0[2].button & 0x10)
        fn_1_3AEEC(fileId, volume, -120, 120);
    if (pad_status_groups->unk0[4].button & 8)
    {
        fileId--;
        if (fileId <= -1)
            fileId = 6;
    }
    if (pad_status_groups->unk0[4].button & 4)
    {
        fileId++;
        if (fileId >= 7)
            fileId = 0;
    }
    if (pad_status_groups->unk0[0].stickY != 0)
    {
        volume += pad_status_groups->unk0[0].stickY / 20;
        fn_1_3A340(2, volume);
    }
    if (pad_status_groups->unk0[0].stickX != 0)
    {
        pan += pad_status_groups->unk0[0].stickX / 20;
        fn_1_3A45C(2, pan);
        pan = fn_1_3B6EC(2);
    }
}

void fn_8_171C4(void)
{
    int real;
    int time;
    int min;
    int sec;
    int frame;
    int total;
    int totalMin;
    int totalSec;
    int totalFrame;
    int pct;
    int i;
    char *newStatus;

    debug_textdraw_set_pos(2, 2);
    debug_textdraw_put_buf_1("ADX test\n");
    debug_textdraw_put_buf_1("\n");
    debug_textdraw_vsprintf_buf_1("file ID   : %d/%d\n", fileId, 6);
    debug_textdraw_vsprintf_buf_1("file name : %s\n", fn_1_3B790(fileId));
    real = fn_1_3B64C(2);
    debug_textdraw_vsprintf_buf_1("volume    : %3.1fdB(%3d), real: %3.1fdB(%3d)\n",
        volume / 10.0, volume, real / 10.0, real);
    debug_textdraw_vsprintf_buf_1("pan       : %d\n", pan);
    time = fn_1_3B408(2);
    min = time / 3600;
    sec = (time / 60) % 60;
    frame = time % 60;
    total = fn_1_3B514(2);
    totalMin = total / 3600;
    totalSec = (total / 60) % 60;
    totalFrame = total % 60;
    pct = time * 100 / total;
    debug_textdraw_vsprintf_buf_1("play time : %dmin %2dsec %2dframe (%d%)",
        min, sec, frame, pct);
    if (paused != 0 && (frame_counter & 0x20))
        debug_textdraw_put_buf_1(" PAUSE");
    debug_textdraw_vsprintf_buf_1("\ntotal time: %dmin %2dsec %2dframe\n", totalMin, totalSec, totalFrame);
    newStatus = fn_1_3B7D0(2);
    if (strncmp(status[0], newStatus, 0x28) != 0)
    {
        for (i = 6; i > 0; i--)
            strcpy(status[i], status[i - 1]);
        strcpy(status[0], newStatus);
    }
    debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("\nstatus: %s\n", status[0]);
    for (i = 1; i < 7; i++)
    {
        u8 c = 3.0 * (1.0 - (float)i / 7.0f);

        debug_textdraw_set_color((c << 6) | (c << 4) | (c << 2) | 3);
        debug_textdraw_vsprintf_buf_1("      : %s\n", status[i]);
    }
    debug_textdraw_set_color(0xFF);
    debug_textdraw_vsprintf_buf_1("buffer size: 0x%08x bytes\n", fn_1_3B97C());
    debug_textdraw_put_buf_1("\n");
    debug_textdraw_put_buf_1("button A   : start\n");
    debug_textdraw_put_buf_1("button B   : stop\n");
    debug_textdraw_put_buf_1("button Y   : pause & resume\n");
    debug_textdraw_put_buf_1("button X   : cross fade\n");
    debug_textdraw_put_buf_1("button Z   : cut in\n");
    debug_textdraw_put_buf_1("button U&D : select\n");
    debug_textdraw_put_buf_1("stick U&D  : volume\n");
    debug_textdraw_put_buf_1("stick L&R  : pan\n");
    debug_textdraw_put_buf_1("trigger L&R: fade-out & -in\n");
}

void fn_8_175C0(void)
{
    fn_1_3A26C();
}
