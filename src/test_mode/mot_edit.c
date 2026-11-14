#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "camera.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "va.h"

typedef s64 OSTime;

typedef struct OSCalendarTime
{
    /*0x00*/ int sec;
    /*0x04*/ int min;
    /*0x08*/ int hour;
    /*0x0C*/ int mday;
    /*0x10*/ int mon;
    /*0x14*/ int year;
    /*0x18*/ int wday;
    /*0x1C*/ int yday;
    /*0x20*/ int msec;
    /*0x24*/ int usec;
} OSCalendarTime;

struct SkelName
{
    u8 unk0[0x14];
    /*0x14*/ char *name;
};

struct ModelName
{
    /*0x00*/ char *name;
    u8 unk4[0x18];
};

struct MotLists
{
    /*0x00*/ struct SkelName *skels;
    /*0x04*/ u32 skelCount;
    /*0x08*/ struct ModelName *models;
    /*0x0C*/ u32 modelCount;
};

extern int currentHeap;
extern char *lbl_1_bss_C8AE4;
extern struct MotLists *lbl_1_bss_C8AE8;
extern int lbl_1_bss_C8AEC;
extern char **lbl_1_bss_C8AF0;
struct CharaGma
{
    struct GMA *gma;
    u8 unk4[4];
};

extern struct CharaGma lbl_1_bss_C8AC4[];
extern char *lbl_1_data_51190[];

int sprintf(char *buf, const char *fmt, ...);
char *strcpy(char *dst, const char *src);
int strncmp(const char *a, const char *b, u32 n);
int strlen(const char *s);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_set_color(int color);
void debug_textdraw_put_buf_1(char *str);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void OSFreeToHeap(int heap, void *ptr);
OSTime OSGetTime(void);
void OSTicksToCalendarTime(OSTime t, OSCalendarTime *cal);
int fn_8006174C(void);
void fn_80061754(char *name, char *buf);
int fn_800617C8(char *path, int mode);
void fn_800618B8(int file);
void fn_80061914(int file, void *buf, int size);
void fn_800619F4(int file, void *buf, int size);
void fn_1_CE618(struct Ape *ape, int a, int b);
void fn_1_D1F24(void);
void fn_1_D1F28(struct Ape *ape);
void fn_1_D1F84(int *category, int *status);
void fn_1_D2510(struct Ape *ape);
int fn_1_D288C(void (*func)(void *, int), void *arg, int list);
void fn_1_D292C(void);
void fn_1_D29E0(int id);
struct Ape *load_ape_gameplay_id_only(int chara);
void mtxa_to_quat(Quat *q);
void g_set_up_debugtext_buffer(void);

static u8 lbl_8_data_5F08[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0xFE, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00, 0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0F, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x0D, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

static char *categoryNames[] = {
    "Move", "Suspend", "Special", "Drop", "Coli", "Goal", "Advertise", "Ending",
    "Selector", "Lose", "Etc", "Race&Fight", "Pilot", "Golf", "Bowling", "Billiard",
    NULL,
};

static char *statusMove[] = {
    "Very Snail", "Snail", "Very Slow", "More Slow", "Slow Walk", "Walk", "Speedy Walk",
    "Fast Walk", "Harried Step", "Run", "Harried Run", "Fast Run", "All One's Power",
    "Back Step", NULL,
};

static char *statusSuspend[] = {
    "Blink", "Looking", "Appeal", "Sulk", "Sulk Sit", "Continue?", NULL,
};

static char *statusSpecial[] = {
    "Caution Front Stop", "Caution Front Walk", "Caution Front Run",
    "Caution Back  Stop", "Caution Back  Walk", "Caution Back  Run",
    "Caution Left  Stop", "Caution Left  Walk", "Caution Left  Run",
    "Caution Right Stop", "Caution Right Walk", "Caution Right Run", "Start",
    "Continue Accept", "Name Entry", "Bonus Stage", "Continue GiveUp", NULL,
};

static char *statusDrop[] = {
    "Standard Drop", NULL,
};

static char *statusColi[] = {
    "Slowly Hit", "Normal Hit", "Hispeed Hit", "Crash", NULL,
};

static char *statusGoal[] = {
    "Rank 1 Loop", "Rank 1", "Rank 2 Loop", "Rank 2", "Rank 3 Loop", "Rank 3",
    "Rank 4 Loop", "Rank 4", "Rank 5 Loop", "Rank 5", "Warp Loop", "Warp",
    "No Miss Clear", NULL,
};

static char *statusAdvertise[] = {
    "BLUESKY 00", "NIGHT 00", "ICE SAMUI START", "ICE SAMUI LOOP", "ICE SAMUI & SLIP",
    "ICE STOP", "SABAKU WALK 00", "SABAKU WALK 01", "SABAKU SIT", "DRF WALK START",
    "DRF STOP", "DRF BIKKURI", "BALL RUN", "SPACE", "tmp 2", "tmp 3", NULL,
};

static char *statusSelector[] = {
    "Stand", "Decide", "Start chara on", "Start chara off", "Select", "No select", NULL,
};

static char *statusLose[] = {
    "Bonus Failed", "Time Over", "Battle Lose", "Battle Lose2", "Lose", "Lose", NULL,
};

static char *statusEtc[] = {
    "Winner", "2nd", "3rd", "4th", "Self Attack", "Winner2", "2nd2", "3rd2", "4th2",
    NULL, "reserve5", "reserve6", "reserve7", "reserve8", "reserve9", NULL,
};

static char *statusRaceFight[] = {
    "crouching", "banana slip", "caught down", "burned", "benumbed", "first", "2nd",
    "3rd", "4th", "get set!", "banana slip loop", "caught down loop", NULL, "reserve1",
    "reserve2", "reserve3", "reserve4", "reserve5", "reserve6", "reserve7", "reserve8",
    NULL,
};

static char *statusPilot[] = {
    "flight-flight", "flight-in ball", "TORNADO-flight", "TORNADO-in ball",
    "BOM hit-flight", "BOM hit-in ball", "BOM_falldown", "BOM ball falldown",
    "flight-flight-L", "flight-flight-R", NULL, "reserve1", "reserve2", "reserve3",
    "reserve4", "reserve5", "reserve6", "reserve7", "reserve8", "reserve9", "reserve10",
    "reserve11", "reserve12", "reserve13", "reserve14", "reserve15", "reserve16", NULL,
};

static char *statusGolf[] = {
    "standard", "for set", "set angle", "for swing", "stance", "swing MAX", "swing",
    "swing MIN", "hole in one", "bogey", NULL, "reserve1", "reserve2", "reserve3",
    "reserve4", "reserve5", "reserve6", "reserve7", "reserve8", "reserve9", "reserve10",
    "reserve11", "reserve12", "reserve13", "reserve14", "reserve15", "reserve16",
    "reserve17", "reserve18", "reserve19", "reserve20", NULL,
};

static char *statusBowling[] = {
    "Take Bowl", "Set Adjust1", "Set Adjust2", "Throw MAX", "Throw", "Throw MIN",
    "Take Bowl Run", NULL, "reserve1", "reserve2", "reserve3", "reserve4", "reserve5",
    "reserve6", "reserve7", "reserve8", "reserve9", "reserve10", "reserve11",
    "reserve12", "reserve13", NULL,
};

static char *statusBilliard[] = {
    "Shot MAX", "Shot", "Shot MIN", "Set", NULL,
};

static char *statusEnding[] = {
    "EZ/on BANANA", "EZ/speak", "EZ/bye", "NR/flight", "NR/break ball", "NR/fly sky",
    "NR/fly speak", "NR/fly away", "HD/look sky & loop", "HD/open door", "HD/no open",
    "HD/attack", "HD/blink", "HD/walk", "HD/sulk", "HD/eat BANANA",
    "HD/camera glance & loop", "HD/camera interpolate", NULL,
    "MT/unknown", NULL,
};

static char **statusLists[] = {
    statusMove, statusSuspend, statusSpecial, statusDrop, statusColi, statusGoal,
    statusAdvertise, statusEnding, statusSelector, statusLose, statusEtc,
    statusRaceFight, statusPilot, statusGolf, statusBowling, statusBilliard,
};

static char *patternFace[] = {"Smile Face", "Angry Face", "Afraid Face", "Dissut", "Open Mouth", NULL};
static char *patternHandR[] = {"Hand Open", NULL};
static char *patternHandL[] = {"Hand Open", NULL};
static char *patternRotate[] = {"Rotate", NULL};
static char **patternLists[] = {patternFace, patternHandL, patternHandR, patternRotate, patternRotate};
static int patternFaceIds[] = {0, 1, 4, 5, 6};
static int patternHandLIds[] = {2};
static int patternHandRIds[] = {3};
static int patternRotateIds[] = {7};
static int *patternIds[] = {patternFaceIds, patternHandLIds, patternHandRIds, patternRotateIds, patternRotateIds};

struct MenuPage
{
    void (*func)(int);
    char *label;
};

void fn_8_1531C(int a);
void fn_8_161E0(int a);
void fn_8_16258(int a);
void fn_8_16214(int a);

static struct MenuPage menuPages[] = {
    {fn_8_1531C, "Change Chara"},
    {fn_8_161E0, "Change Status"},
    {fn_8_16258, "Save"},
    {fn_8_16214, "Exit"},
    {NULL, NULL},
};

struct MotInfo
{
    /*0x00*/ float unk0;
    /*0x04*/ int unk4;
    /*0x08*/ int unk8;
    /*0x0C*/ int flags;
    /*0x10*/ int motion;
    /*0x14*/ int interp;
    /*0x18*/ float speed;
    u8 unk1C[4];
};

struct MotSave
{
    u8 unk0[0x2000];
    struct MotInfo defaults;
};

struct PatInfo
{
    /*0x00*/ int id;
    /*0x04*/ int unk4;
    /*0x08*/ int value;
    /*0x0C*/ int unkC;
    /*0x10*/ int category;
    u8 unk14[0xC];
};

// These runs are arrays in the original, but a bss array can't sit between
// initialized statics here, so they are spelled as consecutive scalars.
static int padButton0 = 0;
static int padButton1 = 0;
static int padButton2 = 0;
static int padButton3 = 0;
#define padButtons (&padButton0)
static int lastDir = 0;
static int repeatTimer = 0;
static int listChanged = 0;
static int listOpen = 0;
static int listSel = 0;
static int listSaved = 0;
static char **listItems = 0;
static int listX = 0;
static int listY = 0;
static void (*subFunc)(void) = 0;
static int curPage = 0;
static int pageCount = 0;
static char **skelNames = 0;
static char **modelNames = 0;
static struct Ape *curApe = 0;
static int curChara = 0;
static float apeRot = 0.0f;
static int curCategory = 0;
static int curStatus0 = 0;
static int curStatus1 = 0;
static int curStatus2 = 0;
static int curStatus3 = 0;
static int curStatus4 = 0;
static int curStatus5 = 0;
static int curStatus6 = 0;
static int curStatus7 = 0;
static int curStatus8 = 0;
static int curStatus9 = 0;
static int curStatus10 = 0;
static int curStatus11 = 0;
static int curStatus12 = 0;
static int curStatus13 = 0;
static int curStatus14 = 0;
static int curStatus15 = 0;
#define curStatus (&curStatus0)
static struct MotInfo *curInfo = 0;
static float curSpeed = 0.0f;
static int *motionIds = 0;
static char **motionNames = 0;
static int rowCount = 0;
static int rowCursor = 0;
static struct PatInfo *curPat = 0;
static int patLimit = 0;
static int patState = 0;
static int eventId = 0;
static int running = 0;
static int unkMode = 0;
static int spinOn = 0;
static s16 spinAngle = 0;

void fn_8_14E54(void);
void fn_8_155B4(void);
void fn_8_159DC(void);

static inline int take_a(void)
{
    int r = padButtons[0];

    padButtons[0] = 0;
    return r;
}

static inline int take_b(void)
{
    int r = padButtons[1];

    padButtons[1] = 0;
    return r;
}

#define DIR_HELD(m) \
    ((pad_status_groups->unk0[4].button & (m)) || (analog_inputs[0][4] & (m)) \
     || (((pad_status_groups->unk0[0].button & (m)) || (analog_inputs[0][0] & (m))) \
         && (analog_inputs[0][0] & 0x200)))

int fn_8_14284(void)
{
    int pressed = 0;

    if (DIR_HELD(8))
    {
        pressed = -1;
        lastDir = 1;
    }
    else if (DIR_HELD(4))
    {
        pressed = -1;
        lastDir = 2;
    }
    else if (DIR_HELD(1))
    {
        pressed = -1;
        lastDir = 3;
    }
    else if (DIR_HELD(2))
    {
        pressed = -1;
        lastDir = 4;
    }
    if (pressed == -1)
    {
        repeatTimer = 30;
        return lastDir;
    }
    if (--repeatTimer > 0)
        return 0;
    if (DIR_HELD(8) && lastDir == 1)
        pressed = -1;
    else if (DIR_HELD(4) && lastDir == 2)
        pressed = -1;
    else if (DIR_HELD(1) && lastDir == 3)
        pressed = -1;
    else if (DIR_HELD(2) && lastDir == 4)
        pressed = -1;
    if (pressed == -1)
    {
        repeatTimer = 4;
        return lastDir;
    }
    lastDir = 0;
    repeatTimer = 0;
    return 0;
}

static inline void fill_str(char *buf, int c, int len)
{
    int i;

    for (i = 0; i < len; i++)
        buf[i] = c;
    buf[i] = 0;
}

void fn_8_147B4(void)
{
    char buf[80];
    int n;
    int width;
    int top;
    int prevSel;
    int dir;
    int i;

    listChanged = 0;
    if (listOpen == 0)
        return;
    if (take_b())
    {
        listOpen = 0;
        listSel = listSaved;
        listChanged = 1;
        return;
    }
    if (take_a())
    {
        listOpen = 0;
        return;
    }
    prevSel = listSel;
    width = 0;
    n = 0;
    while (listItems[n] != NULL)
    {
        int len = strlen(listItems[n]);

        if (len > width)
            width = len;
        n++;
    }
    width += 4;
    dir = fn_8_14284();
    if (dir == 1)
    {
        if (--listSel < 0)
            listSel = n - 1;
    }
    else if (dir == 2)
    {
        if (++listSel >= n)
            listSel = 0;
    }
    if (n > 24)
    {
        int total = n;

        if (dir == 3)
        {
            if ((listSel -= 10) < 0)
                listSel = 0;
        }
        else if (dir == 4)
        {
            if ((listSel += 10) >= n)
                listSel = n - 1;
        }
        n = 24;
        if (listSel > total - 12)
            top = total - 24;
        else if (listSel < 12)
            top = 0;
        else
            top = listSel - 12;
    }
    else
        top = 0;
    debug_textdraw_set_color(0x33);
    debug_textdraw_set_pos(listX, listY + n + 3);
    fill_str(buf, '-', width);
    debug_textdraw_vsprintf_buf_1("+%s+\n", buf);
    debug_textdraw_set_pos(listX, listY);
    fill_str(buf, '-', width);
    debug_textdraw_vsprintf_buf_1("+%s+\n", buf);
    fill_str(buf, ' ', width);
    for (i = 0; i < n + 2; i++)
        debug_textdraw_vsprintf_buf_1("|%s|\n", buf);
    debug_textdraw_set_color(0xFF);
    debug_textdraw_set_pos(listX + 2, listY + 2);
    for (i = 0; i < n; i++)
    {
        char *fmt;

        if (listSel == i + top)
        {
            debug_textdraw_set_color(0xC3);
            fmt = ">%s<\n";
        }
        else
        {
            debug_textdraw_set_color(0xFF);
            fmt = " %s\n";
        }
        debug_textdraw_vsprintf_buf_1(fmt, listItems[i + top]);
    }
    debug_textdraw_set_color(0xFF);
    listChanged = listSel != prevSel;
}

void fn_8_14B14(void)
{
    int dir;
    int i;

    debug_textdraw_set_pos(10, 0);
    debug_textdraw_vsprintf_buf_1("MOTION EDITOR");
    debug_textdraw_set_pos(11, 1);
    debug_textdraw_vsprintf_buf_1("Use Pattern %d/%d\n", *(int *)(lbl_1_bss_C8AE4 + 0x28000),
                                  ((u32)(lbl_1_bss_C8AE4 + 0x4E000) - (u32)(lbl_1_bss_C8AE4 + 0x28000)) / 16);
    debug_textdraw_vsprintf_buf_1("Save Memory %x\n", lbl_1_bss_C8AE4);
    if (subFunc == NULL)
    {
        dir = fn_8_14284();
        if (dir == 1)
        {
            if (--curPage < 0)
                curPage = pageCount - 1;
        }
        else if (dir == 2)
        {
            if (++curPage >= pageCount)
                curPage = 0;
        }
    }
    if (subFunc == NULL)
    {
        debug_textdraw_set_pos(23, 3);
        debug_textdraw_vsprintf_buf_1("     -- MENU --\n\n");
        for (i = 0; i < pageCount; i++)
        {
            if (i == curPage && subFunc == NULL)
            {
                debug_textdraw_set_color(0x33);
                debug_textdraw_vsprintf_buf_1("-> ");
            }
            else
            {
                debug_textdraw_set_color(0xFF);
                debug_textdraw_vsprintf_buf_1("   ");
            }
            debug_textdraw_vsprintf_buf_1("%s\n", menuPages[i].label);
        }
    }
    debug_textdraw_set_color(0xFF);
}

// The next three functions are unused and get dropped, but their literals stay.
void unused_toggle_row(void)
{
    debug_textdraw_put_buf_1("mot_edit.c");
    debug_textdraw_put_buf_1(" ON");
    debug_textdraw_put_buf_1("OFF");
}

void fn_8_14CEC(char *label, char *fmt, ...)
{
    va_list args;
    char buf[92];

    if (rowCursor == rowCount)
    {
        debug_textdraw_set_color(0x33);
        debug_textdraw_put_buf_1("-> ");
    }
    else
    {
        debug_textdraw_set_color(0xFF);
        debug_textdraw_put_buf_1("   ");
    }
    debug_textdraw_put_buf_1(label);
    if (rowCursor == rowCount)
        debug_textdraw_set_color(0x33);
    else
        debug_textdraw_set_color(0xC3);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    debug_textdraw_put_buf_1(buf);
    rowCount++;
    debug_textdraw_set_color(0xFF);
    debug_textdraw_put_buf_1("\n");
}

void unused_int_row(void)
{
    debug_textdraw_put_buf_1("->");
    debug_textdraw_put_buf_1("  ");
    debug_textdraw_put_buf_1("%3d");
    debug_textdraw_put_buf_1("  %-10s");
    debug_textdraw_put_buf_1("  %-15s %3d");
    debug_textdraw_put_buf_1("Program Set");
}

static inline void clear_status(void)
{
    u32 i;

    for (i = 0; i < 16; i++)
        curStatus[i] = 0;
}

static inline void reset_chara(void)
{
    struct MotSave *save;

    if (curApe != NULL)
        fn_1_D1F28(curApe);
    curApe = load_ape_gameplay_id_only(curChara);
    apeRot = 16384.0f;
    mtxa_from_identity();
    mtxa_rotate_y(apeRot);
    mtxa_to_quat(&curApe->rot);
    if (curChara == 3)
        curApe->pos.y = 0.4f;
    else
        curApe->pos.y = 0.38f;
    curApe->unk264.x = curApe->unk264.y = curApe->unk264.z = 0.0f;
    curCategory = 0;
    clear_status();
    save = (struct MotSave *)(lbl_1_bss_C8AE4 + (curChara << 12));
    curInfo = &save->defaults;
    curSpeed = curInfo->speed;
}

static inline void row_nav_reset(void)
{
}

static inline int row_nav(void)
{
    int dir;

    if (listOpen != 0)
        dir = 0;
    else
    {
        dir = fn_8_14284();
        if (dir == 1)
        {
            if (--rowCursor < 0)
                rowCursor = rowCount - 1;
        }
        else if (dir == 2)
        {
            if (++rowCursor >= rowCount)
                rowCursor = 0;
        }
    }
    return dir;
}

static inline void find_in_list(char **names, char *cur)
{
    int len = strlen(cur);
    int i;

    listItems = names;
    for (i = 0; names[i] != NULL; i++)
    {
        if (strncmp(cur, names[i], len) == 0)
        {
            listSel = i;
            listSaved = i;
            return;
        }
    }
    listSaved = listSel = 0;
}

void fn_8_14E54(void)
{
    int reload = 0;
    int dir;

    if (listOpen == 0 && take_b())
    {
        subFunc = NULL;
        return;
    }
    rowCount = 0;
    debug_textdraw_set_pos(3, 7);
    debug_textdraw_vsprintf_buf_1("--- Character Menu ---");
    debug_textdraw_set_pos(1, 8);
    fn_8_14CEC("ID : ", "%d", curChara);
    fn_8_14CEC("Skeleton: ", "%s", lbl_1_bss_C8AE4 + curChara * 0xB4);
    fn_8_14CEC("Model   : ", "%s", lbl_1_bss_C8AE4 + curChara * 0xB4 + 0x18);
    dir = row_nav();
    switch (rowCursor)
    {
    case 0:
        reload = curChara;
        if (dir == 3)
        {
            if (--curChara < 0)
                curChara = 3;
        }
        else if (dir == 4)
        {
            if (++curChara > 3)
                curChara = 0;
        }
        reload = reload != curChara;
        break;
    case 1:
        if (listOpen == 0 && take_a())
        {
            find_in_list(skelNames, lbl_1_bss_C8AE4 + curChara * 0xB4);
            listOpen = 1;
        }
        if (listChanged != 0)
        {
            reload = 1;
            strcpy(lbl_1_bss_C8AE4 + curChara * 0xB4, skelNames[listSel]);
        }
        break;
    case 2:
        if (listOpen == 0 && take_a())
        {
            find_in_list(modelNames, lbl_1_bss_C8AE4 + curChara * 0xB4 + 0x18);
            listOpen = 1;
        }
        if (listChanged != 0)
        {
            reload = 1;
            strcpy(lbl_1_bss_C8AE4 + curChara * 0xB4 + 0x18, modelNames[listSel]);
        }
        break;
    }
    if (reload != 0)
        reset_chara();
}

void fn_8_1531C(int a)
{
    if (subFunc == NULL && a != 0)
        subFunc = fn_8_14E54;
}

void unused_step_scale(float *out)
{
    out[0] = 0.1f;
    out[1] = 10.0f;
    out[2] = 100.0f;
}

void unused_step_table(int *ints)
{
    int table[16] = {0xFF, 0xCF, 0xCF, 0x33, 0x33, 0x33, 0x10, 8, 0x20, 6, 9, 0x10, 0x18, 4, 6, 4};

    ints[0] = table[0];
}

void fn_8_15350(void)
{
    struct PatInfo *pat = curPat;

    mtxa_push();
    mtxa_from_identity();
    mtxa_translate_xyz(0.0f, -0.3f, -1.0f);
    switch (pat->id)
    {
    case 0:
    case 1:
    case 4:
    case 5:
    case 6:
        mtxa_rotate_z(-0x4000);
        mtxa_rotate_y(-0x4000);
        mtxa_scale_s(1.5f);
        load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
        fn_1_CE618(curApe, pat->id, pat->value);
        break;
    case 2:
    case 3:
        debug_textdraw_set_pos(15, 20);
        debug_textdraw_vsprintf_buf_1("Not Assign Patter\n  Press X button.\n");
        break;
    case 7:
    {
        mtxa_translate_xyz(0.0f, 0.2f, 0.0f);
        mtxa_rotate_z(-0x4000);
        mtxa_rotate_y(-0x4000);
        mtxa_rotate_z(182.04445f * pat->value);
        mtxa_scale_s(1.5f);
        debug_textdraw_set_pos(15, 18);
        debug_textdraw_vsprintf_buf_1("Rotate: %d\n", pat->value);
        {
            int table[8] = {2, 4, 2, 4, 2, 4, 2, 4};
            struct GMAModelEntry *entries = lbl_1_bss_C8AC4[curApe->charaId].gma->modelEntries;
            int idx = curApe->charaId * 2;

            if (curPat->category == 4)
            {
                idx++;
                mtxa_translate_xyz(0.0f, 0.12f, 0.0f);
            }
            else
                mtxa_translate_xyz(0.0f, -0.12f, 0.0f);
            idx = table[idx];
            load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
            avdisp_draw_model_unculled_sort_never(entries[idx].model);
        }
        break;
    }
    }
    mtxa_pop();
}

void fn_8_155B4(void)
{
    int dir;
    struct PatInfo *pat;

    if (listOpen == 0 && patState == 0 && take_b())
    {
        subFunc = fn_8_159DC;
        rowCursor = 0;
        return;
    }
    rowCount = 0;
    dir = row_nav();
    if (listChanged != 0)
    {
        switch (patState)
        {
        case 1:
            curPat->category = listSel;
            break;
        case 2:
            curPat->id = patternIds[curPat->category][listSel];
            break;
        }
    }
    if (listOpen == 0)
    {
        if (patState == 1)
        {
            if (curPat->category <= 4)
            {
                listSaved = listSel = 0;
                listItems = patternLists[curPat->category];
                listOpen = 1;
                curPat->id = patternIds[curPat->category][0];
                patState = 2;
            }
        }
        else if (patState == 2)
        {
            curPat->value = 0;
            if (curPat->id < 8)
                patLimit = 16;
            patState = 3;
        }
    }
    if (listOpen != 0 || patState == 3)
    {
        if (patState == 2)
        {
            listX = 16;
            listY = 9;
        }
        else if (patState == 3)
        {
            if (curPat->id != 7)
            {
                debug_textdraw_set_pos(25, 21);
                debug_textdraw_vsprintf_buf_1("Pattern: %3d", curPat->value);
                pat = curPat;
                if (dir == 3)
                {
                    if (--pat->value < 0)
                        pat->value = patLimit - 1;
                }
                else if (dir == 4)
                {
                    if (++pat->value >= patLimit)
                        pat->value = 0;
                }
            }
            else
            {
                pat = curPat;
                if (dir == 3)
                {
                    if (--pat->value < -90)
                        pat->value = -90;
                }
                else if (dir == 4)
                {
                    if (++pat->value > 90)
                        pat->value = 90;
                }
            }
            if (take_a())
                patState = 0;
        }
        return;
    }
    if (pad_status_groups->unk0[2].button & 0x400)
    {
        listSaved = listSel = 0;
        listItems = lbl_1_data_51190;
        listOpen = 1;
        patState = 1;
        curPat = (struct PatInfo *)(lbl_1_bss_C8AE4 + 0x28000)
                 + ((int *)(lbl_1_bss_C8AE4 + 0x20000))[*(int *)(lbl_1_bss_C8AE4 + 0x28000)];
        curPat->category = 0;
        curPat->id = 0;
        curPat->unkC = 0;
        curPat->value = 0;
        return;
    }
    patState = 0;
}

static inline void edit_toggle_flag(int bit)
{
    if (take_a())
        curInfo->flags ^= bit;
}

void fn_8_159DC(void)
{
    int dir;

    if (listOpen == 0 && take_b())
    {
        subFunc = NULL;
        return;
    }
    rowCount = 0;
    debug_textdraw_set_pos(4, 7);
    debug_textdraw_vsprintf_buf_1("--- Category Menu ---");
    debug_textdraw_set_pos(1, 8);
    fn_8_14CEC("Category      : ", "%s", categoryNames[curCategory]);
    fn_8_14CEC("Status        : ", "%s", statusLists[curCategory][curStatus[curCategory]]);
    fn_8_14CEC("Motion        : ", "%s", lbl_1_bss_C8AF0[curInfo->motion]);
    fn_8_14CEC("Float         : ", "%0.2f", curInfo->unk0);
    fn_8_14CEC("Int 1         : ", "%d", curInfo->unk4);
    fn_8_14CEC("Int 2 or Loop : ", "%d", curInfo->unk8);
    fn_8_14CEC("Interp        : ", "%d", curInfo->interp);
    fn_8_14CEC("Speed         : ", "%0.4f", curInfo->speed);
    { int flags = curInfo->flags; fn_8_14CEC("Face Dir      :   ", "%s", (flags & 1) ? " ON" : "OFF"); }
    { int flags = curInfo->flags; fn_8_14CEC("Protect       : ", "%s", (flags & 2) ? " ON" : "OFF"); }
    { int flags = curInfo->flags; fn_8_14CEC("Calc Interp   : ", "%s", (flags & 4) ? " ON" : "OFF"); }
    { int flags = curInfo->flags; fn_8_14CEC("Rate Interp   : ", "%s", (flags & 8) ? " ON" : "OFF"); }
    { int flags = curInfo->flags; fn_8_14CEC("Loop Motion   : ", "%s", (flags & 16) ? " ON" : "OFF"); }
    fn_8_14CEC(" Pattern Assign", "");
    dir = row_nav();
    switch (rowCursor)
    {
    case 0:
        if (listOpen == 0 && take_a())
        {
            listSaved = listSel = curCategory;
            listItems = categoryNames;
            listOpen = 1;
        }
        if (listChanged != 0)
        {
            curCategory = listSel;
            return;
        }
        break;
    case 1:
        if (listOpen == 0 && take_a())
        {
            int sel = curStatus[curCategory];

            listSel = sel;
            listSaved = sel;
            listItems = statusLists[curCategory];
            listOpen = 1;
        }
        if (listChanged != 0)
        {
            curStatus[curCategory] = listSel;
            return;
        }
        break;
    case 2:
        if (listOpen == 0 && take_a())
        {
            find_in_list(motionNames, lbl_1_bss_C8AF0[curInfo->motion]);
            listOpen = 1;
        }
        if (listChanged != 0)
        {
            curInfo->motion = motionIds[listSel];
            return;
        }
        break;
    case 3:
    {
        float *val = &curInfo->unk0;
        float step = 0.1f;

        if (padButtons[2] != 0)
            step *= 10.0f;
        if (padButtons[3] != 0)
            step *= 100.0f;
        if (dir == 3)
        {
            step = *val - step;
            *val = step;
            if (step < -10000.0f)
                *val = -10000.0f;
        }
        else if (dir == 4)
        {
            step = *val + step;
            *val = step;
            if (step > 10000.0f)
                *val = 10000.0f;
        }
        break;
    }
    case 4:
    case 5:
    {
        int *val = &curInfo->unk4 + (rowCursor - 4);
        int step = 1;
        int next;

        if (padButtons[2] != 0)
            step *= 10;
        if (padButtons[3] != 0)
            step *= 100;
        if (dir == 3)
        {
            next = *val - step;
            *val = next;
            if (next < -10000)
                *val = -10000;
        }
        else if (dir == 4)
        {
            next = *val + step;
            *val = next;
            if (next > 10000)
                *val = 10000;
        }
        break;
    }
    case 6:
    {
        int *val = &curInfo->interp;

        if (dir == 3)
        {
            if (--*val < 0)
                *val = 30;
        }
        else if (dir == 4)
        {
            if (++*val > 30)
                *val = 0;
        }
        break;
    }
    case 7:
    {
        float d = (float)pad_status_groups->unk0[0].triggerLeft / 20000.0f;

        d = (float)pad_status_groups->unk0[0].triggerRight / 20000.0f - d;

        if (pad_status_groups->unk0[0].button & 0x100)
            d *= 0.3f;
        curSpeed += d;
        if (curSpeed > 1.0f)
            curSpeed = 1.0f;
        else if (curSpeed < 0.0f)
            curSpeed = 0.0f;
        curInfo->speed = curSpeed;
        return;
    }
    case 8:
        if (take_a())
        {
            curInfo->flags ^= 1;
            return;
        }
        break;
    case 9:
        if (take_a())
        {
            curInfo->flags ^= 2;
            return;
        }
        break;
    case 10:
        if (take_a())
        {
            curInfo->flags ^= 4;
            return;
        }
        break;
    case 11:
        if (take_a())
        {
            curInfo->flags ^= 8;
            return;
        }
        break;
    case 12:
        if (take_a())
        {
            curInfo->flags ^= 16;
            return;
        }
        break;
    case 13:
        if (take_a())
        {
            subFunc = fn_8_155B4;
            rowCursor = 0;
        }
        break;
    }
}

void fn_8_161E0(int a)
{
    if (subFunc == NULL && a != 0)
        subFunc = fn_8_159DC;
}

void fn_8_16214(int a)
{
    if (a != 0)
    {
        fn_1_D29E0(eventId);
        running = 0;
    }
}

void fn_8_16258(int a)
{
    char user[0x80];
    char path1[0x40];
    OSCalendarTime cal1;
    char path2[0x40];
    OSCalendarTime cal2;
    int file;
    void *buf;

    if (a == 0)
        return;
    if (fn_8006174C() == 0)
        return;
    fn_80061754("USER", user);
    if (strcmp(user, "ball") == 0)
    {
        buf = alloc_from_heap_or_panic(currentHeap, 0x4E000, "mot_edit.c", 0x5D4);
        OSTicksToCalendarTime(OSGetTime(), &cal1);
        sprintf(path1, "x:/des/motinfo_%02d%02d%02d%02d%02d.bin", cal1.mon + 1, cal1.mday, cal1.hour, cal1.min, cal1.sec);
        file = fn_800617C8("x:/des/motinfo_usb.bin", 1);
        fn_80061914(file, buf, 0x4E000);
        fn_800618B8(file);
        file = fn_800617C8(path1, 2);
        fn_800619F4(file, buf, 0x4E000);
        fn_800618B8(file);
        OSFreeToHeap(currentHeap, buf);
        file = fn_800617C8("x:/des/motinfo.bin", 2);
    }
    else if (strcmp(user, "y_suzuki") == 0)
    {
        buf = alloc_from_heap_or_panic(currentHeap, 0x4E000, "mot_edit.c", 0x5E8);
        OSTicksToCalendarTime(OSGetTime(), &cal2);
        sprintf(path2, "/p006/prj006/yoshi/motinfo_%02d%02d%02d%02d%02d.bin", cal2.year, cal2.mon + 1, cal2.mday, cal2.hour, cal2.min, cal2.sec);
        file = fn_800617C8("/p006/prj006/yoshi/motinfo.bin", 1);
        fn_80061914(file, buf, 0x4E000);
        fn_800618B8(file);
        file = fn_800617C8(path2, 2);
        fn_800619F4(file, buf, 0x4E000);
        fn_800618B8(file);
        OSFreeToHeap(currentHeap, buf);
        file = fn_800617C8("/p008/p1/yoshi/motinfo.bin", 2);
    }
    else
        file = fn_800617C8("/p008/p1/mkb2/motinfo.bin", 2);
    fn_800619F4(file, lbl_1_bss_C8AE4, 0x4E000);
    fn_800618B8(file);
}

void fn_8_164C4(void *arg0, int arg1)
{
    int quit = arg1 == 3;

    quit |= main_mode != 3;
    quit |= sub_mode != 0x8B;
    if (quit)
    {
        fn_1_D1F28(curApe);
        OSFreeToHeap(currentHeap, modelNames);
        OSFreeToHeap(currentHeap, skelNames);
        OSFreeToHeap(currentHeap, motionIds);
        OSFreeToHeap(currentHeap, motionNames);
        if (arg1 != 3)
            fn_1_D292C();
        return;
    }
}

void fn_8_165C4(void)
{
    u32 i;
    int k;
    int j;

    g_set_up_debugtext_buffer();
    eventId = fn_1_D288C(fn_8_164C4, NULL, 5);
    subFunc = NULL;
    listOpen = 0;
    rowCursor = 0;
    curChara = 0;
    running = 1;
    unkMode = -1;
    curApe = NULL;
    lastDir = 0;
    listChanged = 0;
    curCategory = 0;
    patState = 0;
    clear_status();
    padButtons[0] = padButtons[1] = padButtons[2] = padButtons[3] = 0;
    pageCount = 0;
    while (menuPages[pageCount].label != NULL)
        pageCount++;
    motionIds = alloc_from_heap_or_panic(currentHeap, lbl_1_bss_C8AEC * 4, "mot_edit.c", 0x289);
    motionNames = alloc_from_heap_or_panic(currentHeap, (lbl_1_bss_C8AEC + 1) * sizeof(char *), "mot_edit.c", 0x28A);
    for (j = 0; j < lbl_1_bss_C8AEC; j++)
    {
        motionIds[j] = j + 1;
        motionNames[j] = lbl_1_bss_C8AF0[j + 1];
        for (k = j - 1; k >= 0; k--)
        {
            if (strcmp(motionNames[k], motionNames[k + 1]) <= 0)
                break;
            {
                char *s = motionNames[k];
                int id;

                motionNames[k] = motionNames[k + 1];
                motionNames[k + 1] = s;
                id = motionIds[k];
                motionIds[k] = motionIds[k + 1];
                motionIds[k + 1] = id;
            }
        }
    }
    motionNames[j] = NULL;
    camera_set_state(3);
    g_current_camera->eye.x = 0.0f;
    g_current_camera->eye.y = 0.6f;
    g_current_camera->eye.z = 1.2f;
    g_current_camera->lookAt.x = 0.0f;
    g_current_camera->lookAt.y = 0.5f;
    g_current_camera->lookAt.z = 0.0f;
    fn_1_D1F24();
    skelNames = alloc_from_heap_or_panic(currentHeap, (lbl_1_bss_C8AE8->skelCount + 1) * sizeof(char *), "mot_edit.c", 0x237);
    for (i = 0; i < lbl_1_bss_C8AE8->skelCount; i++)
        skelNames[i] = lbl_1_bss_C8AE8->skels[i].name;
    skelNames[i] = NULL;
    modelNames = alloc_from_heap_or_panic(currentHeap, (lbl_1_bss_C8AE8->modelCount + 1) * sizeof(char *), "mot_edit.c", 0x23D);
    for (i = 0; i < lbl_1_bss_C8AE8->modelCount; i++)
        modelNames[i] = lbl_1_bss_C8AE8->models[i].name;
    modelNames[i] = NULL;
    reset_chara();
}

int fn_8_16B04(void)
{
    int status;
    int category;
    int trigger;

    padButtons[0] = pad_status_groups->unk0[2].button & 0x100;
    padButtons[1] = pad_status_groups->unk0[2].button & 0x200;
    padButtons[2] = pad_status_groups->unk0[0].button & 0x800;
    padButtons[3] = pad_status_groups->unk0[0].button & 0x400;
    fn_8_14B14();
    mtxa_from_identity();
    {
        int spin = pad_status_groups->unk0[0].substickX * 10;

        apeRot += spin;
    }
    mtxa_rotate_y(apeRot);
    mtxa_to_quat(&curApe->rot);
    listX = 3;
    listY = 2;
    if (subFunc != NULL)
        subFunc();
    else
        rowCursor = 0;
    fn_8_147B4();
    if (pad_status_groups->unk0[2].button & 1)
    {
        if (--curApe->colorIndex < 0)
            curApe->colorIndex = 3;
    }
    else if (pad_status_groups->unk0[2].button & 2)
    {
        if (++curApe->colorIndex > 3)
            curApe->colorIndex = 0;
    }
    fn_1_D2510(curApe);
    trigger = 0;
    {
        int active = 0;

        if (menuPages[curPage].func != NULL && subFunc == NULL)
            active = 1;
        if (active && take_a())
            trigger = 1;
    }
    menuPages[curPage].func(trigger);
    if (curCategory == 7 && curStatus[curCategory] >= *(int *)(lbl_1_bss_C8AE4 + curChara * 0xB4 + curCategory * 4 + 0x70))
    {
        category = curCategory;
        status = curStatus[curCategory];
        fn_1_D1F84(&category, &status);
        curInfo = *(struct MotInfo **)(lbl_1_bss_C8AE4 + curChara * 0xB4 + category * 4 + 0x30) + status;
    }
    else
        curInfo = *(struct MotInfo **)(lbl_1_bss_C8AE4 + curChara * 0xB4 + curCategory * 4 + 0x30) + curStatus[curCategory];
    return running;
}

void fn_8_16E24(void)
{
    if (patState == 3)
        fn_8_15350();
    if (pad_status_groups->unk0[2].button & 0x40)
        spinOn ^= 1;
    if (spinOn != 0)
    {
        mtxa_push();
        mtxa_from_mtxb();
        mtxa_translate_xyz(0.0f, 0.5f, 0.0f);
        mtxa_rotate_z(spinAngle / 4);
        mtxa_rotate_x(spinAngle);
        spinAngle += 0x20;
        mtxa_pop();
    }
}
