/* adx.c: plays CRI ADX audio files on two channels (music and jingles), with
 * volume fades and crossfades. */
#include "types.h"
#include "libc.h"
#include "dip.h"

/* CRI ADX library (main.dol) */
void fn_8005B630(int a);                              /* ADXGC_Init? */
void g_create_some_threads(int a);                    /* ADXM_SetupThrd */
void fn_80058C08(void);
void fn_8005A3DC(void (*cb)(void *, char *), void *obj);  /* ADXM_SetCbErr */
void *fn_8005AF8C(int maxch, void *work, int worksize);   /* ADXT_Create */
int fn_8005AB60(void *adxt);                          /* ADXT_GetStat */
void fn_8005AB6C(void *adxt);                         /* ADXT_Stop */
void fn_8005A62C(void *adxt, int vol);                /* ADXT_SetOutVol */
void fn_8005A770(void *adxt, int *ncount, int *tscale);  /* ADXT_GetTime */
void fn_8005B334(void *adxt, char *fname);            /* ADXT_StartFname */
void g_adxt_set_out_pan(void *adxt, int ch, int pan); /* ADXT_SetOutPan */
void fn_8005A3FC(void *adxt, int pause);              /* ADXT_Pause */
int fn_8005A734(void *adxt);                          /* ADXT_GetNumSmpl */
int fn_8005A6F8(void *adxt);                          /* ADXT_GetSfreq */
int fn_8005A624(void *adxt);                          /* ADXT_GetOutVol */
int fn_8005A658(void *adxt, int ch);                  /* ADXT_GetOutPan */
BOOL DVDChangeDir(char *dir);

enum
{
    ADXT_STAT_STOP,
    ADXT_STAT_DECINFO,
    ADXT_STAT_PREP,
    ADXT_STAT_PLAYING,
    ADXT_STAT_DECEND,
    ADXT_STAT_PLAYEND,
};

#define ADX_WORK_SIZE 0x231E4

struct AdxChannel
{
    /*0x00000*/ void *handle;
    /*0x00004*/ u8 work[ADX_WORK_SIZE];
    /*0x231E8*/ u32 flags;  /* 1: playing, 2: stop after fade, 4: fading in, 8: fade back after */
    /*0x231EC*/ s32 volume;
    /*0x231F0*/ float curVolume;
    /*0x231F4*/ s16 targetVolume;
    /*0x231F6*/ u16 fadeFrames;
    /*0x231F8*/ u16 nextFadeFrames;
}; /* size = 0x231FC */

s32 lbl_1_bss_263A8;  /* current channel */
struct AdxChannel lbl_1_bss_263AC[2];

char *lbl_1_data_242B0[] =
{
    "music48a.adx",
    "music48b.adx",
    "music48c.adx",
    "voice48a.adx",
    "voice48b.adx",
    "voice48c.adx",
    "sel_world.adx",
};

void event_adx_init(void);
void g_adx_error_call_back(void *obj, char *msg);

/* Not in the game (stripped at link): makes lbl_1_bss_263A8 the first .bss object, as in
 * the original, whose first use of it was presumably in a stripped function. */
s32 *adx_bss_order(void)
{
    return &lbl_1_bss_263A8;
}

void fn_1_39CE8(void)
{
    fn_8005B630(0);
    g_create_some_threads(0);
    fn_80058C08();
    fn_8005A3DC(g_adx_error_call_back, NULL);
    event_adx_init();
}

void fn_1_3A26C(void);
void fn_1_3A650(int ch, int frames);
void fn_1_3A1C4(int ch);

void event_adx_init(void)
{
    int i;
    struct AdxChannel *adx = lbl_1_bss_263AC;

    fn_1_3A26C();
    for (i = 0; i < 2; i++, adx++)
    {
        if (adx->handle == NULL)
        {
            adx->handle = fn_8005AF8C(2, adx->work, ADX_WORK_SIZE);
            if (adx->handle == NULL)
                g_adx_error_call_back(NULL, "ADX_EventInit: ADXT_Create failed");
        }
        memset(adx->work, 0, ADX_WORK_SIZE);
        adx->flags = 0;
        adx->volume = 0;
        adx->curVolume = 0.0f;
        adx->targetVolume = 0;
        adx->fadeFrames = 0;
        adx->nextFadeFrames = 0;
    }
    lbl_1_bss_263A8 = 0;
}

void event_adx_tick(void)
{
    int i;
    struct AdxChannel *adx = lbl_1_bss_263AC;
    int stat;

    if (g_some_other_flags & 0xA)
        return;
    for (i = 0; i < 2; i++, adx++)
    {
        if (adx->handle == NULL || adx->flags == 0)
            continue;
        stat = fn_8005AB60(adx->handle);
        if (stat == ADXT_STAT_STOP || stat == ADXT_STAT_PLAYEND)
        {
            if (adx->flags & 8)
            {
                fn_1_3A650(lbl_1_bss_263A8, adx->nextFadeFrames);
                adx->nextFadeFrames = 0;
            }
            adx->flags = 0;
            adx->fadeFrames = 0;
        }
        if (adx->fadeFrames == 0)
            continue;
        adx->curVolume += (adx->targetVolume - adx->curVolume) / adx->fadeFrames;
        adx->fadeFrames--;
        fn_8005A62C(adx->handle, adx->curVolume);
        if (adx->fadeFrames == 0)
        {
            if (adx->flags & 2)
            {
                fn_1_3A1C4(i);
                adx->flags &= ~2;
            }
            else if (adx->flags & 4)
                adx->flags &= ~4;
        }
    }
}

void event_adx_dest(void) {}

void fn_1_3A340(int ch, s32 volume);

/* Play an ADX file on a channel (2: the current channel). */
void fn_1_3A040(int ch, int fileIdx)
{
    struct AdxChannel *adx;
    int stat;
    int ncount;
    int tscale;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
        {
            adx = &lbl_1_bss_263AC[ch];
            lbl_1_bss_263A8 = ch;
        }
        if (adx->handle == NULL)
        {
            adx->handle = fn_8005AF8C(2, adx->work, ADX_WORK_SIZE);
            if (adx->handle == NULL)
            {
                g_adx_error_call_back(NULL, "ADX_Start: ADXT_Create failed");
                return;
            }
        }
        else
        {
            stat = fn_8005AB60(adx->handle);
            if (stat == ADXT_STAT_DECINFO || stat == ADXT_STAT_PREP || stat == ADXT_STAT_PLAYING)
            {
                fn_8005A770(adx->handle, &ncount, &tscale);
                if (ncount * 22 < tscale)
                    return;
            }
            fn_1_3A1C4(ch);
        }
        fn_1_3A340(lbl_1_bss_263A8, adx->volume);
        DVDChangeDir("adx");
        fn_8005B334(adx->handle, lbl_1_data_242B0[fileIdx]);
        DVDChangeDir("../");
        adx->flags |= 1;
    }
}

void fn_1_3A1C4(int ch)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
        {
            fn_8005AB6C(adx->handle);
            adx->flags = 0;
            adx->fadeFrames = 0;
        }
    }
}

void fn_1_3A26C(void)
{
    int i;

    for (i = 0; i < 2; i++)
        fn_1_3A1C4(i);
    lbl_1_bss_263A8 = 0;
}

void fn_1_3A340(int ch, s32 volume)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
        {
            adx->volume = (volume < -999) ? -999 : (volume > 0) ? 0 : volume;
            adx->curVolume = adx->volume;
            adx->targetVolume = adx->volume;
            adx->fadeFrames = 0;
            adx->flags &= ~6;
            fn_8005A62C(adx->handle, adx->volume);
        }
    }
}

void fn_1_3A45C(int ch, int pan)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
        {
            if (pan != -128)
                pan = (pan < -15) ? -15 : (pan > 15) ? 15 : pan;
            g_adxt_set_out_pan(adx->handle, 0, pan);
            g_adxt_set_out_pan(adx->handle, 1, pan);
        }
    }
}

void fn_1_3A538(int ch)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
            fn_8005A3FC(adx->handle, 1);
    }
}

void fn_1_3A5C4(int ch)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
            fn_8005A3FC(adx->handle, 0);
    }
}

/* Fade a channel back to its set volume. */
void fn_1_3A650(int ch, int frames)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
        {
            adx->targetVolume = adx->volume;
            adx->fadeFrames = (frames > 1) ? frames : 1;
        }
    }
}

/* Fade a channel to a volume. */
static inline void adx_fade_to(int ch, int volume, int frames)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL)
        {
            adx->targetVolume = (volume < -999) ? -999 : (volume > 0) ? 0 : volume;
            adx->fadeFrames = (frames > 1) ? frames : 1;
        }
    }
}

/* Fade a channel out and stop it. */
void fn_1_3A6E8(int ch, int frames)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        if (adx->handle != NULL && adx->flags != 0)
        {
            if (adx->volume <= -120)
                fn_1_3A1C4(ch);
            else
            {
                adx->flags |= 2;
                adx_fade_to(ch, -120, frames);
            }
        }
    }
}

/* Start a file faded in from silence to a volume. */
void fn_1_3A8B4(int ch, int fileIdx, int volume, int frames)
{
    struct AdxChannel *adx;

    if (ch > -1 && ch < 3)
    {
        if (ch == 2)
            adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
        else
            adx = &lbl_1_bss_263AC[ch];
        fn_1_3A040(ch, fileIdx);
        adx_fade_to(ch, volume, frames);
        adx->curVolume = -120.0f;
        adx->flags |= 4;
    }
}

int fn_1_3B8DC(void);

/* Crossfade from the current channel to a new file on the other channel. */
void fn_1_3ACD8(int fileIdx, int volume, int frames)
{
    fn_1_3A6E8(lbl_1_bss_263A8, frames);
    fn_1_3A8B4(fn_1_3B8DC(), fileIdx, volume, frames);
}

/* Duck the current channel and play a file on the other one, fading the current
 * channel back in when it ends. */
void fn_1_3AEEC(int fileIdx, int volume, int duckVolume, int frames)
{
    int prevChannel;
    int other;

    adx_fade_to(lbl_1_bss_263A8, duckVolume, frames);
    prevChannel = lbl_1_bss_263A8;
    other = fn_1_3B8DC();
    fn_1_3A040(other, fileIdx);
    fn_1_3A340(other, volume);
    lbl_1_bss_263AC[other].flags |= 8;
    lbl_1_bss_263AC[other].nextFadeFrames = frames;
    lbl_1_bss_263A8 = prevChannel;
}

/* Playback position, in frames. */
int fn_1_3B408(int ch)
{
    struct AdxChannel *adx;
    int ncount;
    int tscale;

    if (ch <= -1)
        return 0;
    if (ch >= 3)
        return 0;
    if (ch == 2)
        adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
    else
        adx = &lbl_1_bss_263AC[ch];
    if (adx->handle == NULL)
        return 0;
    fn_8005A770(adx->handle, &ncount, &tscale);
    return (float)(ncount * 60) / (float)tscale;
}

/* Length of the playing file, in frames. */
int fn_1_3B514(int ch)
{
    struct AdxChannel *adx;
    int stat;
    int samples;
    int rate;

    if (ch <= -1)
        return 0;
    if (ch >= 3)
        return 0;
    if (ch == 2)
        adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
    else
        adx = &lbl_1_bss_263AC[ch];
    if (adx->handle == NULL)
        return 0;
    stat = fn_8005AB60(adx->handle);
    if (stat == ADXT_STAT_STOP || stat == ADXT_STAT_DECINFO)
        return 0;
    samples = fn_8005A734(adx->handle);
    rate = fn_8005A6F8(adx->handle);
    return (float)(samples * 60) / (float)rate;
}

int fn_1_3B64C(int ch)
{
    struct AdxChannel *adx;

    if (ch <= -1)
        return 0;
    if (ch >= 3)
        return 0;
    if (ch == 2)
        adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
    else
        adx = &lbl_1_bss_263AC[ch];
    if (adx->handle == NULL)
        return 0;
    return fn_8005A624(adx->handle);
}

int fn_1_3B6EC(int ch)
{
    struct AdxChannel *adx;

    if (ch <= -1)
        return 0;
    if (ch >= 3)
        return 0;
    if (ch == 2)
        adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
    else
        adx = &lbl_1_bss_263AC[ch];
    if (adx->handle == NULL)
        return 0;
    return fn_8005A658(adx->handle, 0);
}

char *fn_1_3B790(int fileIdx)
{
    if (fileIdx <= -1)
        return "data ID is illegal";
    if (fileIdx >= 7)
        return "data ID is illegal";
    return lbl_1_data_242B0[fileIdx];
}

char *fn_1_3B7D0(int ch)
{
    struct AdxChannel *adx;
    int stat;
    char *names[] =
    {
        "ADXT_STAT_STOP (stop)",
        "ADXT_STAT_DECINFO (decode header info)",
        "ADXT_STAT_PREP (preparing for play)",
        "ADXT_STAT_PLAYING (decode & play)",
        "ADXT_STAT_DECEND (decode end)",
        "ADXT_STAT_PLAYEND (play end)",
    };

    if (ch <= -1)
        return "handle ID is illegal";
    if (ch >= 3)
        return "handle ID is illegal";
    if (ch == 2)
        adx = &lbl_1_bss_263AC[lbl_1_bss_263A8];
    else
        adx = &lbl_1_bss_263AC[ch];
    if (adx->handle == NULL)
        return "adx handle is null";
    stat = fn_8005AB60(adx->handle);
    /* (sizeof(names) instead of the entry count, as in the original) */
    if (stat < 0 || stat > sizeof(names))
        return "unknown status";
    return names[stat];
}

/* The channel to use for a new sound: an idle one, else one fading out, else the
 * one that isn't current. */
int fn_1_3B8DC(void)
{
    int i;
    struct AdxChannel *adx;

    for (i = 0, adx = lbl_1_bss_263AC; i < 2; i++, adx++)
    {
        if (adx->flags == 0)
            return i;
    }
    for (i = 0, adx = lbl_1_bss_263AC; i < 2; i++, adx++)
    {
        if (adx->flags & 3)
            return i;
    }
    return (lbl_1_bss_263A8 + 1) % 2;
}

int fn_1_3B97C(void)
{
    return ADX_WORK_SIZE * 2;
}

void g_adx_error_call_back(void *obj, char *msg)
{
    if (dip_switches & DIP_DEBUG)
    {
        printf("adx.c:\n");
        printf("  adx_error_call_back received message:\n");
        printf("    %s\n", msg);
    }
}
