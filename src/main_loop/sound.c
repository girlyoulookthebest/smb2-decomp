/* Sound effects, music and voice (MusyX and ADX streams). */
#include "types.h"
#include "libc.h"
#include "va.h"
#include "file.h"
#include "futsal_data.h"
#include "ball.h"
#include "dip.h"
#include "mode.h"
#include "camera.h"
#include "event.h"
#include "stage.h"
#include "mathutil.h"
#include "sound.h"

/* Name and file of a streamed sound. */
struct SoundFileName
{
    /*0x00*/ char *name;
    /*0x04*/ char *file;
    /*0x08*/ s8 volume;
    /*0x09*/ u8 unk9;
    /*0x0A*/ u16 flags;  /* 1: volume not managed by the crossfader, 2/4: left/right channel */
};

/* Streamed music (ADX), by name. */
struct SelWorldSound
{
    /*0x00*/ char *name;
    /*0x04*/ u8 volume;
    /*0x08*/ char *file;
    /*0x0C*/ float frameRate;
    /*0x10*/ u8 unk10;  /* 2: mono file, 4: not music, 8: alias of the previous track */
}; /* size = 0x14 */

#include "sound_tables.inc"
#include "sound_tables2.inc"

/* Default effect settings (reverb: coloration, mix, time, damping, pre-delay, crosstalk;
 * chorus: base delay, variation, period). */
/* Default MusyX aux effect settings. */
struct ReverbSettings
{
    float coloration;
    float mix;
    float time;
    float damping;
    float preDelay;
    float crosstalk;
};

struct ChorusSettings
{
    u32 baseDelay;
    u32 variation;
    u32 period;
};

const struct ReverbSettings lbl_1_rodata_700C = { 0.9f, 0.65f, 3.0f, 0.6f, 0.1f, 0.0f };
const struct ChorusSettings lbl_1_rodata_7024 = { 15, 0, 500 };

extern void *currentHeap;

/* .bss (one pooled block from g_debug_sound_ram_usage) */
s32 g_debug_sound_ram_usage;   /* bytes of sound data loaded */
s32 g_debug_sound_aram_usage;
u32 lbl_1_bss_15F28[2];         /* ARAM block list for ARInit */
u32 lbl_1_bss_15F30[10];  /* smgr stream per port */
s16 g_active_music_tracks[10];
s8 g_something_related_to_bgm_track_id[10];
s32 lbl_1_bss_15F78[10];
struct SoundTrackVolume
{
    /*0x00*/ u32 duration;  /* fade length in frames */
    /*0x04*/ u32 timer;
    /*0x08*/ s8 vol1;  /* volume at fade start */
    /*0x09*/ s8 vol2;  /* current volume (percent) */
    /*0x0A*/ s8 delta;  /* volume change over the fade */
} lbl_1_bss_15FA0[10];
s8 g_some_music_related_counter[10];
s8 lbl_1_bss_16024[10];  /* port state */
u8 lbl_1_bss_1602E;  /* master volume (percent) */
u8 lbl_1_bss_1602F;
float lbl_1_bss_16030;
float lbl_1_bss_16034;
float lbl_1_bss_16038;
float lbl_1_bss_1603C;
s32 lbl_1_bss_16040[16];  /* sound group slot loaded */
s32 lbl_1_bss_16080;      /* number of loaded sound groups */
s32 lbl_1_bss_16084[8];   /* loaded sound groups, in load order */
u8 lbl_1_bss_160A4;
u8 lbl_1_bss_160A5;
u32 g_something_with_sound5[8][0x5B8];  /* voice IDs, per ball and sound effect */
s16 lbl_1_bss_217A8;
struct
{
    s32 unk0;
    s32 unk4;
} lbl_1_bss_217AC[8];
s32 lbl_1_bss_217EC;  /* current sound group set */
s32 lbl_1_bss_217F0;
char *lbl_1_bss_217F4;
char *lbl_1_bss_217F8;
s8 lbl_1_bss_217FC;
s8 lbl_1_bss_217FD;
char lbl_1_bss_21800[0x80];
void *s_poolData[16];
u32 s_poolDataSizes[16];
void *s_projData[16];
u32 s_projDataSizes[16];
void *s_sdirData[16];
u32 s_sdirDataSizes[16];
s32 s_groupIsUsingCharaHeap;
void *s_sampData[16];
u32 s_sampDataSizes[16];
s8 lbl_1_bss_21A84;
struct SoundParaReq
{
    /*0x00*/ s16 unk0;     /* sound effect to play */
    /*0x02*/ s16 soundId;  /* sound effect slot for voice tracking */
    /*0x04*/ s8 volume;
    /*0x05*/ s8 pan;
    /*0x06*/ s8 span;
    /*0x07*/ s8 pitch;
    /*0x08*/ s32 playerId;
} lbl_1_bss_21A88[16];
s8 lbl_1_bss_21B48[8][8];
s32 g_smth_with_sound;
s32 g_player_id_for_sound;
s8 lbl_1_bss_21B90[8][0x5B8];  /* per-ball sound effect volume */
s8 lbl_1_bss_24950[8];  /* per-ball pan */
s8 lbl_1_bss_24958[8];  /* per-ball span */
float lbl_1_bss_24960[8];  /* per-ball volume */
s8 lbl_1_bss_24980[0x5B8];  /* per-sound volume adjustment */
float lbl_1_bss_24F38[8];  /* ball volume when an ICS sound last updated (for doppler) */
s8 lbl_1_bss_24F58[10];  /* paired port (stereo) */
s32 lbl_1_bss_24F64;  /* mute music */
s8 lbl_1_bss_24F68[10];  /* smgr channel per port */
struct File lbl_1_bss_24F74[10];
struct StreamSEReq
{
    s16 id;
    s8 volume;
    s8 pan;
    s8 span;
} lbl_1_bss_25294[10];  /* deferred SoftStreamSEReq */
u32 lbl_1_bss_252D0[10];  /* stream allocated for a streamed SE */

/* Header of a Nintendo DSP-ADPCM file. */
struct DSPHeader
{
    /*0x00*/ u32 numSamples;
    /*0x04*/ u32 numNibbles;
    /*0x08*/ u32 sampleRate;
    /*0x0C*/ u16 loopFlag;
    /*0x0E*/ u16 format;
    /*0x10*/ u32 loopStart;
    /*0x14*/ u32 loopEnd;
    /*0x18*/ u32 ca;
    /*0x1C*/ u16 coefs[16];
    u8 unk3C[0x60 - 0x3C];
};

struct DSPHeader lbl_1_bss_25300[10] __attribute__((aligned(32)));

/* Parameters for starting a stream (see fn_1_378D4). */
struct StreamParams
{
    /*0x00*/ char *fileName;
    /*0x04*/ u16 sampleRate;
    /*0x08*/ u32 dataOffset;
    /*0x0C*/ u32 loopEnd;
    /*0x10*/ u16 coefs[16];
    /*0x30*/ BOOL loopFlag;
    /*0x34*/ u32 loopStart;
    /*0x38*/ float frameRate;
};

struct StreamParams lbl_1_bss_256C0[10];
s8 lbl_1_bss_25918[147];  /* per-track volume adjustment */
BOOL lbl_1_bss_259AC;  /* aux effects active */

/* MusyX aux effect state (only the settings are named). */
typedef struct
{
    u8 unk0[0x1C8];
    struct ReverbSettings settings;
} SND_AUX_REVERBHI;

typedef struct
{
    u8 unk0[0x90];
    struct ChorusSettings settings;
} SND_AUX_CHORUS;

SND_AUX_REVERBHI lbl_1_bss_259B0;
SND_AUX_CHORUS lbl_1_bss_25B90;


typedef struct
{
    void *(*malloc)(u32 size);
    void (*free)(void *ptr);
} SND_HOOKS;

void ARInit(u32 *stackBase, u32 numEntries);
void ARQInit(void);
void AIInit(u8 *stack);
void sndSetHooks(SND_HOOKS *hooks);
int sndInit(u8 voices, u8 music, u8 sfx, u8 studios, u32 flags, u32 aramSize);
void sndVolume(u8 volume, u16 time, u8 volGroup);
void sndMasterVolume(u8 volume, u16 time, u8 music, u8 fx);
void fn_1_375A8(void);
void g_something_with_bgm(void);
void fn_1_343F4(void);
void fn_1_358E8(void);
void g_set_some_sound_data(void);
void fn_1_36DCC(void);
void fn_1_367E4(int mode);
void fn_1_39CE8(void);
void fn_1_33E64(void);
void fn_1_34D34(void);
void fn_1_33418(void);
void g_crossfade_music(void);
s8 fn_1_36534(int a);
char *fn_1_36488(int a);
void hwExit(void);
void *OSAllocFromHeap(void *heap, u32 size);
void *OSSetCurrentHeap(void *heap);
s32 OSCheckHeap(void *heap);
BOOL DVDChangeDir(char *dir);
int sndPushGroup(void *proj, u16 groupId, void *samp, void *sdir, void *pool);
int sndPopGroup(void);
u32 sndFXCheck(u32 vid);
int sndFXKeyOff(u32 vid);
void SoundEffectInit(void);
void fn_1_32338(s8 reverb, s8 chorus);
void fn_1_371C8(void);
void fn_1_372BC(void);
void fn_1_37300(void);
void fn_1_3350C(s16 a);
float g_smth_called_by_event_tick_sound_1(s8 ball, s16 soundId);
void g_smth_called_by_event_tick_sound_2(void);
void g_smth_calls_sndFXStartParaInfo(struct SoundParaReq *req);
void g_call_synthFXSetCtrl_with_irq_disabled(u32 vid, u8 ctrl, u8 value);

typedef struct
{
    u8 ctrl;
    union
    {
        u8 value7;
        u16 value14;
    } paraData;
} SND_PARAMETER;

typedef struct
{
    u8 numPara;
    SND_PARAMETER *paraArray;
} SND_PARAMETER_INFO;

void g_call_synthFXSetCtrl14_with_irq_disabled(u32 vid, u8 ctrl, u16 value);
s16 g_SoftStreamStart_with_some_defaults_2(int track);

u32 sndFXStartParaInfo(u16 fid, u8 vol, u8 pan, u8 studio, SND_PARAMETER_INFO *paraInfo);
int g_check_flag_related_to_pausing(void);

/* A keyframe of a Hermite curve, as evaluated by fn_1_D2550. */
struct Keyframe
{
    s32 frame;
    float value;
    float tangentIn;
    float tangentOut;
};

float fn_1_D2550(float frame, struct Keyframe *curve);
s16 g_maybe_related_to_music_crossfading(int track);
void g_change_music_volume(int track, u32 time, s8 volume);
s16 play_track_and_fade_out_other_tracks(int track, int fadeTime, s8 volume);
void g_something_with_stopping_music_or_sfx(int track);
u32 fn_1_384EC(u32 stream);
int fn_1_38A74(int port);
void fn_1_38618(int channel, s8 volume);
void fn_1_3874C(int channel, s8 volume);
float math_sin(s16 angle);
void SoftStreamSEReq(s8 mode, int id, s8 volume, s8 pan, s8 span);
s16 SoftStreamStart(u8 flags, int track, s8 volume);
u32 g_smgr_allocate_stereo(struct StreamParams *left, struct StreamParams *right, u8 volume, s8 vol2,
                           s8 *channelL, s8 *channelR, int flags);
u32 fn_1_38944(int port);
u32 fn_1_3892C(int port);
int fn_1_38974(int port);
void g_maybe_smth_with_music(int track, s8 volume);
void fn_1_387E0(int channel, s8 pan, s8 span);
u32 fn_1_3895C(int channel);
void fn_1_38888(int channel, int value);
void fn_1_388A0(int port);
#define OS_SOUND_MODE_MONO 0u
#define OS_SOUND_MODE_STEREO 1u
u32 OSGetSoundMode(void);
void OSSetSoundMode(u32 mode);
void fn_8003DCDC(int mode);
void fn_8005205C(u8 reason, void *info, void *user);  /* sndAuxCallbackReverbHI */
void fn_80053430(u8 reason, void *info, void *user);  /* sndAuxCallbackChorus */
BOOL sndAuxCallbackPrepareReverbHI(SND_AUX_REVERBHI *rev);
BOOL sndAuxCallbackPrepareChorus(SND_AUX_CHORUS *ch);
BOOL sndAuxCallbackShutdownChorus(SND_AUX_CHORUS *ch);
BOOL g_something_with_sound8_wrapper(SND_AUX_REVERBHI *rev);
BOOL sndAuxCallbackUpdateSettingsReverbHI(SND_AUX_REVERBHI *rev);
BOOL sndAuxCallbackUpdateSettingsChorus(SND_AUX_CHORUS *ch);
void sndSetAuxProcessingCallbacks(u8 studio, void (*auxA)(u8, void *, void *), void *userA, u8 midiA, u32 seqA,
                                  void (*auxB)(u8, void *, void *), void *userB, u8 midiB, u32 seqB);
extern void *main_heap;
u32 fn_1_378D4(struct StreamParams *params, u8 volume, u8 pan, u8 span, s8 *channelOut, int flags);
s32 DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void (*callback)(s32, void *), s32 prio);
s16 fn_1_34A28(int id);
void g_stop_music_sound_debug(int id);
void call_SoftStreamSEReq_arg_0(int id, s8 volume, s8 pan, s8 span);
void fn_1_389AC(int port, int pause);
s32 DVDGetCommandBlockStatus(void *block);
void fn_1_38220(u32 stream);
char *fn_1_34AB0(int id);
int fn_1_33368(void);
s8 fn_1_2F5B0(int cameraId, Vec *pos, s8 *pan, s8 *span);
s8 fn_1_2F8F0(int a, Vec *pos, s8 *pan, s8 *span);

extern void *chara_heap;
void *try_alloc_memory_for_sound(u32 size);
void g_OSFree_wrapper(void *ptr);

void *alloc_from_heap_or_panic(void *heap, u32 size, char *file, int line);
void OSFreeToHeap(void *heap, void *ptr);

void g_printf_sound_error(s8 isWarning, const char *func, const char *msg, ...)
{
    va_list args;
    char kind[16];
    char buffer[0x200];

    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    strcpy(kind, isWarning == 0 ? "ERROR" : "WARNING");
    printf("### SOUND %s ### at %s", kind, func);
    printf(buffer);
}

/* Read a whole MusyX data file into a new buffer (size rounded up to 32 bytes). */
void *ReadMusyXData(const char *fileName, u32 *sizeOut)
{
    struct File file;
    u32 size;
    void *buffer;

    if (!file_open((char *)fileName, &file))
    {
        g_printf_sound_error(0, "ReadMusyXData\n", "Unable to open '%s'\n", fileName);
        return NULL;
    }
    size = file_size(&file);
    if (size == 0)
    {
        g_printf_sound_error(0, "ReadMusyXData\n", "Zero length file '%s'\n", fileName);
        return NULL;
    }
    *sizeOut = OSRoundUp32B(size);
    buffer = OSAllocFromHeap(currentHeap, *sizeOut);
    if (buffer == NULL)
        return NULL;
    if (file_read(&file, buffer, *sizeOut, 0) <= 0)
    {
        g_printf_sound_error(0, "ReadMusyXData\n", "Failed to read data from '%s'\n", fileName);
        OSFreeToHeap(currentHeap, buffer);
        return NULL;
    }
    file_close(&file);
    return buffer;
}

void *try_alloc_memory_for_sound(u32 size)
{
    return alloc_from_heap_or_panic(currentHeap, size, "sound.c", 553);
}

void g_OSFree_wrapper(void *ptr)
{
    OSFreeToHeap(currentHeap, ptr);
}

void init_sound(void)
{
    SND_HOOKS hooks = { try_alloc_memory_for_sound, g_OSFree_wrapper };
    int i;
    int j;

    g_debug_sound_ram_usage = 0;
    g_debug_sound_aram_usage = 0;
    ARInit(lbl_1_bss_15F28, 2);
    ARQInit();
    AIInit(NULL);
    sndSetHooks(&hooks);
    sndInit(0x30, 0x30, 0x30, 1, 1, 0x780000);
    fn_1_375A8();
    for (i = 0; i < 10; i++)
    {
        lbl_1_bss_15F30[i] = -1;
        g_active_music_tracks[i] = -1;
        g_something_related_to_bgm_track_id[i] = -1;
        lbl_1_bss_15F78[i] = 0;
        lbl_1_bss_15FA0[i].duration = 0;
        lbl_1_bss_15FA0[i].timer = 0;
        lbl_1_bss_15FA0[i].vol1 = 100;
        lbl_1_bss_15FA0[i].vol2 = 100;
        lbl_1_bss_15FA0[i].delta = 0;
    }
    g_something_with_bgm();
    fn_1_343F4();
    fn_1_358E8();
    for (i = 0; i < 10; i++)
    {
        g_some_music_related_counter[i] = -1;
        lbl_1_bss_16024[i] = 0;
    }
    lbl_1_bss_1602E = lbl_1_bss_1602F = 100;
    sndVolume(0.01f * (127.0f * lbl_1_bss_1602E), 0, 0xFF);
    lbl_1_bss_16030 = 1.0f;
    lbl_1_bss_16034 = 1.0f;
    lbl_1_bss_16038 = 1.0f;
    lbl_1_bss_1603C = 1.0f;
    for (i = 0; i < 16; i++)
        lbl_1_bss_16040[i] = 0;
    lbl_1_bss_16080 = 0;
    for (i = 0; i < 8; i++)
        lbl_1_bss_16084[i] = -1;
    g_set_some_sound_data();
    fn_1_36DCC();
    lbl_1_bss_160A4 = 0;
    lbl_1_bss_160A5 = 0;
    sndMasterVolume(0x7F, 0, 1, 1);
    sndVolume(0.01f * (127.0f * lbl_1_bss_1602E), 0, 0xFF);
    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < 0x5B8; i++)
            g_something_with_sound5[j][i] = -1;
    }
    lbl_1_bss_217A8 = -1;
    for (i = 0; i < 8; i++)
        lbl_1_bss_217AC[i].unk0 = 0;
    fn_1_367E4(-1);
    lbl_1_bss_217EC = 0;
    lbl_1_bss_217F0 = 1;
    fn_1_39CE8();
}

void g_something_with_music_fading(void)
{
    fn_1_33E64();
    fn_1_34D34();
    fn_1_33418();
    g_crossfade_music();
    lbl_1_bss_217F4 = lbl_1_rodata_1284[lbl_1_bss_217EC].name;
    lbl_1_bss_217F8 = lbl_1_rodata_148C[lbl_1_bss_217F0].name;
    lbl_1_bss_217FC = fn_1_36534(lbl_1_bss_217FD);
    strcpy(lbl_1_bss_21800, fn_1_36488(lbl_1_bss_217FD));
}

void hwExit_wrapper(void)
{
    hwExit();
}


static inline void free_sound_group_data(const struct SoundGroupDesc *desc)
{
    OSFreeToHeap(currentHeap, s_poolData[desc->slot]);
    g_debug_sound_ram_usage -= s_poolDataSizes[desc->slot];
    OSFreeToHeap(currentHeap, s_projData[desc->slot]);
    g_debug_sound_ram_usage -= s_projDataSizes[desc->slot];
    OSFreeToHeap(currentHeap, s_sdirData[desc->slot]);
    g_debug_sound_ram_usage -= s_sdirDataSizes[desc->slot];
}

/* Load a MusyX sound group (pool, project, sample directory and samples, the samples
 * falling back to the character heap) and push it to the sound system. */
void SoundGroupLoad(int groupId)
{
    char poolFileName[0x100];
    char projFileName[0x100];
    char sampFileName[0x100];
    char sdirFileName[0x100];
    u32 bytesRead;
    void *heap;
    const struct SoundGroupDesc *desc;
    int i;
    int ok;
    u32 freeSpace;

    if (lbl_1_bss_16080 == 8)
        return;
    desc = &lbl_1_rodata_1284[groupId];
    if (lbl_1_bss_16040[desc->slot] != 0)
        return;
    for (i = 0; i < 8; i++)
    {
        if (groupId == lbl_1_bss_16084[i])
            return;
    }

    sprintf(poolFileName, "%s.pool", desc->baseName);
    sprintf(projFileName, "%s.proj", desc->baseName);
    sprintf(sampFileName, "%s.samp", desc->baseName);
    sprintf(sdirFileName, "%s.sdir", desc->baseName);
    DVDChangeDir("snd/mkb");

    s_poolData[desc->slot] = ReadMusyXData(poolFileName, &bytesRead);
    s_poolDataSizes[desc->slot] = bytesRead;
    g_debug_sound_ram_usage += s_poolDataSizes[desc->slot];

    s_projData[desc->slot] = ReadMusyXData(projFileName, &bytesRead);
    s_projDataSizes[desc->slot] = bytesRead;
    g_debug_sound_ram_usage += s_projDataSizes[desc->slot];

    s_sdirData[desc->slot] = ReadMusyXData(sdirFileName, &bytesRead);
    s_sdirDataSizes[desc->slot] = bytesRead;
    g_debug_sound_ram_usage += s_sdirDataSizes[desc->slot];

    s_groupIsUsingCharaHeap = FALSE;
    s_sampData[desc->slot] = ReadMusyXData(sampFileName, &bytesRead);
    if (s_sampData[desc->slot] == NULL)
    {
        s_groupIsUsingCharaHeap = TRUE;
        heap = OSSetCurrentHeap(chara_heap);
        s_sampData[desc->slot] = ReadMusyXData(sampFileName, &bytesRead);
        OSSetCurrentHeap(heap);
        if (s_sampData[desc->slot] == NULL)
        {
            freeSpace = OSCheckHeap(chara_heap);
            if (freeSpace == -1U)
                g_printf_sound_error(0, "SoundGroupLoad\n", "RAM ERROR ! \"%s\"(0x%X) CharaHeap error\n", desc->baseName, bytesRead);
            else if (bytesRead < freeSpace)
                g_printf_sound_error(0, "SoundGroupLoad\n", "RAM ALLOC ERROR ! \"%s\"(0x%X) CharaHeap:0x%X\n", desc->baseName, bytesRead, freeSpace);
            else
                g_printf_sound_error(0, "SoundGroupLoad\n", "RAM SIZE OVER ! \"%s\"(0x%X) need 0x%X ... CharaHeap:0x%X\n", desc->baseName, bytesRead, bytesRead - freeSpace, freeSpace);
            DVDChangeDir("/");
            free_sound_group_data(desc);
            return;
        }
    }

    s_sampDataSizes[desc->slot] = bytesRead;
    g_debug_sound_ram_usage += s_sampDataSizes[desc->slot];
    if (g_debug_sound_aram_usage + s_sampDataSizes[desc->slot] >= 0x6C0000)
    {
        g_printf_sound_error(0, "SoundGroupLoad\n", "ARAM SIZE OVER ! \"%s\"(0x%X) need 0x%X\n", desc->baseName,
                             s_sampDataSizes[desc->slot], g_debug_sound_aram_usage + (s_sampDataSizes[desc->slot] - 0x6C0000));
        if (s_groupIsUsingCharaHeap)
            heap = OSSetCurrentHeap(chara_heap);
        OSFreeToHeap(currentHeap, s_sampData[desc->slot]);
        if (s_groupIsUsingCharaHeap)
            OSSetCurrentHeap(heap);
        g_debug_sound_ram_usage -= s_sampDataSizes[desc->slot];
        DVDChangeDir("/");
        free_sound_group_data(desc);
        return;
    }

    ok = sndPushGroup(s_projData[desc->slot], desc->groupId, s_sampData[desc->slot], s_sdirData[desc->slot], s_poolData[desc->slot]);
    if (s_groupIsUsingCharaHeap)
        heap = OSSetCurrentHeap(chara_heap);
    OSFreeToHeap(currentHeap, s_sampData[desc->slot]);
    if (s_groupIsUsingCharaHeap)
        OSSetCurrentHeap(heap);
    g_debug_sound_ram_usage -= s_sampDataSizes[desc->slot];
    if (ok == 0)
    {
        g_printf_sound_error(0, "sndPushGroup\n", "FAULT! %s\n", desc->baseName);
        DVDChangeDir("/");
        free_sound_group_data(desc);
        return;
    }
    g_debug_sound_aram_usage += s_sampDataSizes[desc->slot];
    lbl_1_bss_16040[desc->slot] = 1;
    DVDChangeDir("/");
    lbl_1_bss_16084[lbl_1_bss_16080++] = groupId;
    return;
}

void g_something_calls_sndPopGroup(void)
{
    const struct SoundGroupDesc *desc;

    if (lbl_1_bss_16080 > 3)
    {
        desc = &lbl_1_rodata_1284[lbl_1_bss_16084[--lbl_1_bss_16080]];
        sndPopGroup();
        g_debug_sound_aram_usage -= s_sampDataSizes[desc->slot];
        free_sound_group_data(desc);
        lbl_1_bss_16040[desc->slot] = 0;
        lbl_1_bss_16084[lbl_1_bss_16080] = -1;
    }
}

void g_something_calls_sndPopGroup_2(void)
{
    const struct SoundGroupDesc *desc;

    if (lbl_1_bss_16080 <= 3 && lbl_1_bss_16080 > 0)
    {
        desc = &lbl_1_rodata_1284[lbl_1_bss_16084[--lbl_1_bss_16080]];
        sndPopGroup();
        g_debug_sound_aram_usage -= s_sampDataSizes[desc->slot];
        g_debug_sound_ram_usage -= s_poolDataSizes[desc->slot];
        g_debug_sound_ram_usage -= s_projDataSizes[desc->slot];
        g_debug_sound_ram_usage -= s_sdirDataSizes[desc->slot];
        lbl_1_bss_16040[desc->slot] = 0;
        lbl_1_bss_16084[lbl_1_bss_16080] = -1;
    }
}

void event_sound_init(void)
{
    struct SoundParaReq *req;
    int i;
    int j;

    lbl_1_bss_21A84 = 0;
    SoundEffectInit();
    fn_1_32338(0, 0);
    for (req = lbl_1_bss_21A88, i = 0; i < 16; i++, req++)
    {
        req->unk0 = -1;
        req->soundId = 0;
        req->volume = 0;
        req->pan = 0;
        req->span = 0;
        req->pitch = 0;
        req->playerId = 0;
    }
    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < 8; i++)
            lbl_1_bss_21B48[j][i] = 0;
    }
    g_smth_with_sound = -1;
    g_player_id_for_sound = -1;
    fn_1_371C8();
}

void event_sound_tick(void)
{
    int j;
    int i;
    u32 *vid;
    float volume;
    float scale;
    const struct SoundEffectDesc *desc;

    for (i = 0; i < 8; i++)
    {
        if (i > 0 && balls[i].status == 0)
            continue;
        for (j = 0; j < 0x5B8; j++)
        {
            vid = &g_something_with_sound5[i][j];
            volume = lbl_1_bss_21B90[i][j];
            if (*vid != -1U)
            {
                *vid = sndFXCheck(*vid);
                scale = g_smth_called_by_event_tick_sound_1(i, j);
                volume *= scale * lbl_1_bss_16030;
                if (lbl_1_bss_16030 == lbl_1_bss_16034)
                {
                    if (1.0f != scale)
                        g_call_synthFXSetCtrl_with_irq_disabled(*vid, 7, volume);
                }
                else
                {
                    desc = &lbl_1_rodata_148C[j];
                    if (!(desc->flags & 0x4000))
                        g_call_synthFXSetCtrl_with_irq_disabled(*vid, 7, volume);
                }
            }
        }
    }
    if (lbl_1_bss_1602E != lbl_1_bss_1602F)
    {
        sndVolume(0.01f * (127.0f * lbl_1_bss_1602E), 0, 0xFF);
        lbl_1_bss_1602F = lbl_1_bss_1602E;
    }
    fn_1_372BC();
    g_smth_called_by_event_tick_sound_2();
    if (!(g_some_other_flags & 0xA))
    {
        struct SoundParaReq *req;

        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (lbl_1_bss_21B48[i][j] > 0)
                    lbl_1_bss_21B48[i][j]--;
            }
        }
        for (req = lbl_1_bss_21A88, j = 0; j < 16; j++, req++)
        {
            if (req->unk0 == -1)
                break;
            g_smth_calls_sndFXStartParaInfo(req);
            req->unk0 = -1;
        }
    }
}

void event_sound_dest(void)
{
    int j;
    int i;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 0x5B8; j++)
        {
            if (g_something_with_sound5[i][j] != -1U && !(lbl_1_rodata_148C[j].flags & 8))
                sndFXKeyOff(g_something_with_sound5[i][j]);
        }
    }
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 0x5B8; j++)
            g_something_with_sound5[i][j] = -1;
    }
    for (j = 0; j < 8; j++)
        lbl_1_bss_217AC[j].unk0 = 0;
    lbl_1_bss_16038 = 1.0f;
    fn_1_37300();
    for (j = 0; j < 10; j++)
    {
        if (g_active_music_tracks[j] >= 0 && g_something_related_to_bgm_track_id[j] == 0
         && (SEL_WORLD_PTR[g_active_music_tracks[j]].unk10 & 4))
            fn_1_3350C(j);
    }
}

/* Lower the volume of a sound effect when other balls are playing it too. */
float g_smth_called_by_event_tick_sound_1(s8 ball, s16 soundId)
{
    float sum = 0.0f;
    int count = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        if (i != ball && g_something_with_sound5[i][soundId] != -1U)
        {
            count++;
            sum += lbl_1_bss_21B90[i][soundId];
        }
    }
    return (count == 0) ? 1.0 : 1.0 - 0.5 * ((float)count / 8.0) * (sum / count / 127.0f);
}

static s8 lbl_1_data_23B74[9] = {
    100, -64, 63,
    -64, 100, 63,
    -64, 63, 100,
};

static s8 lbl_1_data_23B80[8] = { 0, 1, -1, 2, -2, 3, -3, 4 };

int fn_1_2DB44(int playerCount)
{
    switch (modeCtrl.gameType)
    {
    case 1:
    case 3:
    case 5:
    case 10:
    case 13:
        if (playerCount == 3 && modeCtrl.splitscreenMode == 3)
            return 4;
        break;
    }
    return playerCount;
}

void g_smth_called_by_event_tick_sound_2(void)
{
    int i;
    int first = -1;
    int last = 0;
    int r28 = modeCtrl.unk30;
    s8 pan;

    for (i = 0; i < 8; i++)
    {
        lbl_1_bss_24950[i] = 100;
        lbl_1_bss_24958[i] = 100;
        lbl_1_bss_24960[i] = -1.0f;
    }
    if (r28 == 0)
        return;
    if (modeCtrl.gameType == 3 || modeCtrl.gameType == 5 || modeCtrl.gameType == 10 || modeCtrl.gameType == 13)
    {
        first = r28;
        if (modeCtrl.gameType == 3)
            last = 7;
        else
            last = 3;
    }
    else if (modeCtrl.gameType == 4)
    {
        first = 0;
        last = 3;
    }
    if (lbl_1_bss_21A84 == 1)
    {
        first = 0;
        r28 = 1;
    }
    if (first == -1)
        return;
    if (modeCtrl.gameType == 4)
    {
        for (i = first; i <= last; i++)
        {
            if (i > 0 && balls[i].status == 0)
                continue;
            lbl_1_bss_24960[i] = fn_1_2F5B0(0, &balls[i].pos, &lbl_1_bss_24950[i], &lbl_1_bss_24958[i]) / 127.0f;
        }
        return;
    }
    for (i = 0; i < first; i++)
    {
        if (i > 0 && balls[i].status == 0)
            continue;
        switch ((s8)fn_1_2DB44(first))
        {
        default:
            pan = 0;
            break;
        case 3:
            pan = (lbl_1_data_23B74[(s8)i + modeCtrl.splitscreenMode * 3] == 100) ? 0 : lbl_1_data_23B74[(s8)i + modeCtrl.splitscreenMode * 3];
            break;
        case 4:
            pan = ((s8)i & 1) ? 63 : -64;
            break;
        }
        lbl_1_bss_24950[i] = pan;
    }
    for (i = first; i <= last; i++)
    {
        if (i > 0 && balls[i].status == 0)
            continue;
        lbl_1_bss_24960[i] = fn_1_2F8F0(r28, &balls[i].pos, &lbl_1_bss_24950[i], &lbl_1_bss_24958[i]) / 127.0f;
    }
}

void g_set_smth_with_sound(s8 a)
{
    lbl_1_bss_21A84 = a;
}

void g_smth_calls_sndFXStartParaInfo(struct SoundParaReq *req)
{
    SND_PARAMETER_INFO info;
    SND_PARAMETER params[4];
    s8 volume;
    s8 n;
    int finalVol;
    const struct SoundEffectDesc *desc;
    u16 fxId;

    fxId = lbl_1_rodata_148C[req->unk0].fxId;
    desc = &lbl_1_rodata_148C[req->soundId];
    if (desc->flags == 0)
        desc = &lbl_1_rodata_148C[req->unk0];
    if (req->pan > 63)
        req->pan = -(128 - req->pan);
    if (req->span > 63)
        req->span = -(128 - req->span);
    volume = (desc->flags & 0x4000) ? (float)req->volume : req->volume * lbl_1_bss_16030;
    if (volume == 0)
        return;

    n = 0;
    params[n].ctrl = 0x83;
    params[n].paraData.value7 = 127 - (u8)(req->span + 0x40);
    n++;
    if (req->pitch != 0)
    {
        params[n].ctrl = 0x80;
        params[n].paraData.value14 = ((req->pitch << 11) + 0x2000 < 0) ? 0
                                   : ((req->pitch << 11) + 0x2000 > 0x3FFF) ? 0x3FFF
                                   : (req->pitch << 11) + 0x2000;
        n++;
    }
    if (!(desc->flags & 0x80))
    {
        params[n].ctrl = 0x5B;
        params[n].paraData.value7 = lbl_1_bss_160A4;
        n++;
        params[n].ctrl = 0x5D;
        params[n].paraData.value7 = lbl_1_bss_160A5;
        n++;
    }
    info.numPara = n;
    info.paraArray = params;
    finalVol = volume * g_smth_called_by_event_tick_sound_1(req->playerId, req->soundId);
    g_something_with_sound5[req->playerId][req->soundId] =
        sndFXStartParaInfo(fxId, finalVol, req->pan + 0x40, 0, &info);
    lbl_1_bss_21B90[req->playerId][req->soundId] = volume;
    if (dip_switches & DIP_SNDREQ_DISP)
    {
        printf("SNDREQ[%-35s] vol:%3d(%3d) pan:%3d span:%3d\n", lbl_1_rodata_148C[req->unk0].name,
               volume, (s8)finalVol, req->pan, req->span);
        printf("  player_id:%d code:%4d cue(id:%4d/rid:%4d) pitch:%d grp:%X\n", req->playerId, fxId,
               req->unk0, req->soundId, req->pitch, desc->flags);
    }
}

static inline s8 get_ball_pan(int playerId)
{
    s8 pan;

    if (g_check_flag_related_to_pausing())
        pan = 0;
    else if (lbl_1_bss_24950[playerId] != 100)
        pan = lbl_1_bss_24950[playerId];
    else
        pan = 0;
    return pan;
}

static inline s8 get_ball_span(int playerId)
{
    s8 span;

    if (g_check_flag_related_to_pausing())
        span = 0;
    else if (lbl_1_bss_24958[playerId] != 100)
        span = lbl_1_bss_24958[playerId];
    else
        span = 0;
    return span;
}

static inline float get_ball_volume(int playerId)
{
    return g_check_flag_related_to_pausing() ? 1.0f
         : (lbl_1_bss_24960[playerId] != -1.0f) ? lbl_1_bss_24960[playerId] : 1.0f;
}

/* Apply volume, pan and span adjustments to a packed sound request. */
int g_something_with_volume(u32 *soundId, const struct SoundEffectDesc *desc, int playerId)
{
    int vol;
    int r29;
    int pan;
    int ret;

    r29 = (*soundId >> 11) & 0x7F;
    if (r29 > 63)
        r29 = -(128 - r29);
    *soundId &= ~(0x7F << 11);
    if (dip_switches & DIP_SE_VOL_RESET)
        vol = 127;
    else if (desc->flags & 0x800)
        vol = r29 * 2 + 127;
    else
        vol = r29 + 127;
    if (!(desc->flags & 0x800))
    {
        vol = (vol < 0) ? 0 : (vol > 127) ? 127 : vol;
        *soundId |= (vol & 0x7F) << 11;
    }

    if (modeCtrl.unk30 == 0)
        return 0;

    if (desc->flags & 0x800)
    {
        r29 = (*soundId >> 18) & 0x7F;
        *soundId &= ~(0x7F << 18);
        pan = get_ball_pan(playerId);
        if (pan < 0)
            pan += 128;
        *soundId |= ((pan + r29) & 0x7F) << 18;

        r29 = *soundId >> 25;
        *soundId &= ~(0x7F << 25);
        pan = get_ball_span(playerId);
        if (pan < 0)
            pan += 128;
        *soundId |= ((pan + r29) & 0x7F) << 25;

        if (!(dip_switches & DIP_SE_VOL_RESET))
        {
            r29 = (vol * get_ball_volume(playerId) < 0.0f) ? 0.0f
                : (vol * get_ball_volume(playerId) > 127.0f) ? 127.0f
                : vol * get_ball_volume(playerId);
            *soundId |= (r29 & 0x7F) << 11;
        }
    }
    ret = 0;
    if ((desc->flags & 0x800) && modeCtrl.unk30 > 1)
        ret = lbl_1_data_23B80[playerId];
    return ret;
}

/* Returns nonzero if the sound should not be played. */
#pragma auto_inline off
int g_something_calls_sndFXKeyOff(u16 soundId, const struct SoundEffectDesc *desc, int playerId)
{
    int i;
    int j;
    const struct SoundEffectDesc *other;

    if (desc->flags & 0x2000)
        return 1;
    if (desc->flags & 0x100)
    {
        if (g_something_with_sound5[playerId][soundId] != -1U)
            return 1;
    }
    else if (desc->flags & 0x400)
    {
        if (g_something_with_sound5[playerId][soundId] != -1U)
        {
            sndFXKeyOff(g_something_with_sound5[playerId][soundId]);
            g_something_with_sound5[playerId][soundId] = -1;
        }
    }
    else if (desc->flags & 0x40)
    {
        for (i = 0; i < 8; i++)
        {
            for (other = lbl_1_rodata_148C, j = 0; j < 0x5B8; j++, other++)
            {
                if (g_something_with_sound5[i][j] != -1U)
                {
                    if (desc->flags == 0x4040)
                    {
                        if (other->flags != 0x4040)
                            continue;
                    }
                    else if (desc->flags != other->flags)
                        continue;
                    sndFXKeyOff(g_something_with_sound5[i][j]);
                    g_something_with_sound5[i][j] = -1;
                }
            }
        }
    }
    return 0;
}
#pragma auto_inline reset

static int get_sound_player_id(const char *funcName, int slot)
{
    int playerId;

    if ((lbl_1_rodata_148C[slot].flags & 0x10) || (lbl_1_rodata_148C[slot].flags & 0x20))
        return 0;
    if (g_player_id_for_sound == -1 && current_ball == NULL)
    {
        char func[4];

        sprintf(func, "%s\n", funcName);
        g_printf_sound_error(0, func, "\"nowball\" is NULL !! %s for Auto Surround\n", lbl_1_rodata_148C[slot].name);
        return 0;
    }
    if (g_player_id_for_sound != -1)
    {
        playerId = g_player_id_for_sound;
        g_player_id_for_sound = -1;
        return playerId;
    }
    return current_ball->playerId;
}

int SoundReq(u32 soundId)
{
    int playerId;
    struct SoundParaReq *req;
    u16 id;
    u16 slot;
    const struct SoundEffectDesc *desc;
    const struct SoundEffectDesc *slotDesc;
    int i;
    int pitch;

    id = soundId & 0x7FF;
    slot = (lbl_1_bss_217A8 == -1) ? (soundId & 0x7FF) : lbl_1_bss_217A8;
    desc = &lbl_1_rodata_148C[id];
    slotDesc = &lbl_1_rodata_148C[slot];
    if (slotDesc->flags == 0)
    {
        slot = id;
        slotDesc = desc;
    }
    playerId = get_sound_player_id("SoundReq", slot);

    if (g_something_calls_sndFXKeyOff(slot, slotDesc, playerId))
        return -1;
    if ((slotDesc->flags & 0x8000) && (slotDesc->flags & 7) && modeCtrl.gameType != 6
     && lbl_1_bss_21B48[playerId][slotDesc->flags & 7] > 0)
        return -1;

    for (req = lbl_1_bss_21A88, i = 0; i < 16; i++, req++)
    {
        if (req->unk0 == -1)
        {
            pitch = g_something_with_volume(&soundId, slotDesc, playerId);
            req->unk0 = id;
            req->soundId = slot;
            req->volume = (soundId >> 11) & 0x7F;
            req->pan = (soundId >> 18) & 0x7F;
            req->span = soundId >> 25;
            req->pitch = pitch;
            req->playerId = playerId;
            if (slotDesc->flags & 0x8000)
                lbl_1_bss_21B48[playerId][slotDesc->flags & 7] = slotDesc->repeatDelay;
            lbl_1_bss_217A8 = -1;
            req->volume = (req->volume + lbl_1_bss_24980[req->unk0] < 0) ? 0
                        : (req->volume + lbl_1_bss_24980[req->unk0] > 127) ? 127
                        : req->volume + lbl_1_bss_24980[req->unk0];
            return req->soundId;
        }
    }
    lbl_1_bss_217A8 = -1;
    g_printf_sound_error(0, "SoundReq", "%s cue over.", desc->name);
    return -1;
}

int SoundReqDirect(u32 soundId)
{
    int playerId;
    u16 id;
    u16 slot;
    const struct SoundEffectDesc *slotDesc;
    const struct SoundEffectDesc *desc;
    struct SoundParaReq req;
    int pitch;

    id = soundId & 0x7FF;
    slot = (lbl_1_bss_217A8 == -1) ? (soundId & 0x7FF) : lbl_1_bss_217A8;
    desc = &lbl_1_rodata_148C[id];
    slotDesc = &lbl_1_rodata_148C[slot];
    if (slotDesc->flags == 0)
    {
        slot = id;
        slotDesc = desc;
    }
    playerId = get_sound_player_id("SoundReqDirect", slot);

    if (g_something_calls_sndFXKeyOff(slot, slotDesc, playerId))
        return -1;
    pitch = g_something_with_volume(&soundId, slotDesc, playerId);
    req.unk0 = id;
    req.soundId = slot;
    req.volume = (soundId >> 11) & 0x7F;
    req.pan = (soundId >> 18) & 0x7F;
    req.span = soundId >> 25;
    req.pitch = pitch;
    req.playerId = playerId;
    lbl_1_bss_217A8 = -1;
    req.volume = (req.volume + lbl_1_bss_24980[req.unk0] < 0) ? 0
               : (req.volume + lbl_1_bss_24980[req.unk0] > 127) ? 127
               : req.volume + lbl_1_bss_24980[req.unk0];
    g_smth_calls_sndFXStartParaInfo(&req);
    return req.soundId;
}

/* Play a sound by cue ID. Modes 0 and 2 queue the sound, 1 and 3 play it immediately. */
int SoundReqID(u32 soundId, int mode)
{
    int cueId;
    int i;
    int count;
    int result;
    const struct SoundEffectDesc *desc;

    result = -1;
    cueId = soundId & 0x7FF;
    for (desc = lbl_1_rodata_148C, count = 0, i = 0; i < 0x5B8; i++, desc++)
    {
        if (desc->cueId != cueId)
            continue;
        if ((desc->flags & 0x200) && sub_mode == 12)
            return -1;
        if (desc->flags & 0x200)
        {
            count++;
            if (g_smth_with_sound != -1)
            {
                if (g_smth_with_sound + 1 != count)
                    continue;
                g_smth_with_sound = -1;
            }
            else
            {
                if (current_ball->ape == NULL)
                {
                    g_printf_sound_error(0, "SoundReqID\n", "\"nowball\" is NULL !! %s for Auto Surround\n", desc->name);
                    return -1;
                }
                if (current_ball->ape->charaId + 1 != count)
                    continue;
            }
        }
        lbl_1_bss_217A8 = i;
        if (desc->fxId == 0xFFFF)
        {
            for (; i >= 0; i--, desc--)
            {
                if (desc->fxId != 0xFFFF)
                    break;
            }
        }
        soundId &= ~0x7FF;
        soundId |= i;
        switch (mode)
        {
        case 0:
        case 2:
            result = SoundReq(soundId);
            break;
        case 1:
        case 3:
            result = SoundReqDirect(soundId);
            break;
        }
        return result;
    }
    return -1;
}

int call_SoundReqID_arg_0(u32 soundId)
{
    return SoundReqID(soundId, 0);
}

int call_SoundReqID_arg_1(u32 soundId)
{
    return SoundReqID(soundId, 1);
}

int call_SoundReqID_arg_2(u32 soundId)
{
    return SoundReqID(soundId, 2);
}

int call_SoundReqID_arg_1_copy(u32 soundId)
{
    return SoundReqID(soundId, 1);
}

/* Compute volume, pan and span of a sound at pos as heard from a camera. */
s8 fn_1_2F5B0(int cameraId, Vec *pos, s8 *pan, s8 *span)
{
    register float dx;
    float dy;
    register float dz;
    s8 volume;
    struct Camera *camera;
    float dist;
    s16 yaw;

    camera = &cameras[cameraId];
    dx = pos->x - camera->eye.x;
    dy = pos->y - camera->eye.y;
    dz = pos->z - camera->eye.z;
    yaw = math_atan2(dx, dz) - 0x8000;
#ifdef __MWERKS__
    asm
    {
        fmuls dx, dx, dx
        fmadds dx, dz, dz, dx
    }
    math_atan2(dy, math_sqrt(dx));  /* pitch (unused) */
#else
    math_atan2(dy, math_sqrt(dx * dx + dz * dz));
#endif
    yaw -= camera->rotY;
    dist = mathutil_vec_distance(&camera->eye, pos);
    volume = 127.0f * ((1.0f - dist / 100.0f < 0.0f) ? 0.0f
                     : (1.0f - dist / 100.0f > 1.0f) ? 1.0f
                     : 1.0f - dist / 100.0f);
    if (abs(yaw) < 0x4000)
        *pan = 0.0039 * -yaw;
    else if (yaw > 0)
        *pan = 0.0039 * -(0x8000 - yaw);
    else
        *pan = 0.0039 * -(-0x8000 - yaw);
    *span = 0.0039 * -(abs(yaw) - 0x4000);
    *pan = (*pan < -64) ? -64 : (*pan > 63) ? 63 : *pan;
    *span = (*span < -64) ? -64 : (*span > 63) ? 63 : *span;
    return volume;
}

/* Like fn_1_2F5B0, but blends all split-screen cameras, weighting each by distance. */
s8 fn_1_2F8F0(int playerCount, Vec *pos, s8 *pan, s8 *span)
{
    float weights[4];
    float minDist;
    float dist;
    s8 volume = 0;
    int first = 0;
    int i;
    int count;
    s8 camPan;
    s8 camSpan;

    count = fn_1_2DB44(playerCount);
    if (lbl_1_bss_21A84 == 1 && modeCtrl.gameType == 3)
    {
        first = 1;
        count = 2;
    }
    minDist = 10000000.0f;
    for (i = first; i < count; i++)
    {
        dist = mathutil_vec_distance(&cameras[i].eye, pos);
        weights[i] = (1.0f - dist / 100.0f < 0.0f) ? 0.0f
                   : (1.0f - dist / 100.0f > 1.0f) ? 1.0f
                   : 1.0f - dist / 100.0f;
        if (!(dist > minDist))
        {
            minDist = dist;
            volume = 127.0f * weights[i];
        }
    }
    *pan = 0;
    *span = 0;
    switch (count)
    {
    default:
        fn_1_2F5B0(0, pos, pan, span);
        break;
    case 3:
        for (i = 0; i < count; i++)
        {
            fn_1_2F5B0(i, pos, &camPan, &camSpan);
            *pan += camPan * weights[i];
            *span += camSpan * weights[i];
        }
        *pan = *pan / (float)count;
        *span = *span / (float)count;
        break;
    case 4:
        for (i = 0; i < count; i++)
        {
            fn_1_2F5B0(i, pos, &camPan, &camSpan);
            *pan += camPan * weights[i];
            *span += camSpan * weights[i];
        }
        *pan = *pan / (float)count;
        *span = *span / (float)count;
        break;
    }
    return volume;
}

/* NONMATCHING: the table address goes through r0 and an extra `mr r5, r0`; registers match. */
void fn_1_3061C(int soundId)
{
    u32 i;
    struct StreamedSound *entry;
    s16 id = soundId;

    entry = lbl_1_data_1E660;
    id &= 0x7FF;
    for (i = 0; i < 24; i++, entry++)
    {
        if (entry->soundId == id)
        {
            g_SoftStreamStart_with_some_defaults_2(entry->track);
            return;
        }
    }
}

void SoundOff(u16 soundId)
{
    int playerId;

    playerId = get_sound_player_id("SoundOff", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        sndFXKeyOff(g_something_with_sound5[playerId][soundId]);
        g_something_with_sound5[playerId][soundId] = -1;
    }
}

void SoundVol(u16 soundId, u8 volume)
{
    int playerId;

    playerId = get_sound_player_id("SoundVol", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 7, volume);
    }
}

void SoundPan(u16 soundId, s8 pan, s8 span)
{
    int playerId;

    playerId = get_sound_player_id("SoundPan", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 10, (u8)(pan + 0x40));
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 131, (u8)(0x7F - (span + 0x40)));
    }
}

void SoundPitch(u16 soundId, u16 pitch)
{
    int playerId;

    playerId = get_sound_player_id("SoundPitch", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 128, pitch);
    }
}

void SoundDop(u16 soundId, u16 doppler)
{
    int playerId;

    playerId = get_sound_player_id("SoundDop", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 132, doppler);
    }
}

void SoundMod(u16 soundId, u16 mod)
{
    int playerId;

    playerId = get_sound_player_id("SoundMod", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 1, mod);
    }
}

void SoundRev(u16 soundId, u8 reverb)
{
    int playerId;

    playerId = get_sound_player_id("SoundRev", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 91, reverb);
    }
}

void SoundCho(u16 soundId, u8 chorus)
{
    int playerId;

    playerId = get_sound_player_id("SoundCho", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 93, chorus);
    }
}

/* Stripped from the game; only its name string remains. */
BOOL SoundCheckState(u16 soundId)
{
    int playerId;

    playerId = get_sound_player_id("SoundCheckState", soundId);
    return g_something_with_sound5[playerId][soundId] != -1U;
}

/* Find the sound effect for a cue ID, picking the character-specific variant if needed. */
static int search_sound_by_cue(int cueId)
{
    int i;
    int count;
    const struct SoundEffectDesc *desc;

    for (desc = lbl_1_rodata_148C, count = 0, i = 0; i < 0x5B8; i++, desc++)
    {
        if (desc->cueId != cueId)
            continue;
        if (desc->flags & 0x200)
        {
            count++;
            if (g_smth_with_sound != -1)
            {
                if (g_smth_with_sound + 1 != count)
                    continue;
                g_smth_with_sound = -1;
            }
            else
            {
                if (current_ball->ape == NULL)
                {
                    g_printf_sound_error(0, "SoundSearch\n", "\"nowball\" is NULL !! %s for Auto Surround\n", desc->name);
                    return -1;
                }
                if (current_ball->ape->charaId + 1 != count)
                    continue;
            }
        }
        return i;
    }
    return -1;
}

BOOL SoundOffID(int soundId)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundOffID", soundId);
    if (g_something_with_sound5[playerId][soundId] == -1U)
        return FALSE;
    sndFXKeyOff(g_something_with_sound5[playerId][soundId]);
    g_something_with_sound5[playerId][soundId] = -1;
    return TRUE;
}

void SoundVolID(int soundId, u8 volume)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundVolID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 7, volume);
    }
}

void SoundPanID(int soundId, s8 pan, s8 span)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundPanID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 10, (u8)(pan + 0x40));
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 131, (u8)(0x7F - (span + 0x40)));
    }
}

/* SoundPitchID, SoundDopID and SoundModID were stripped from the game; only their names remain. */
void SoundPitchID(int soundId, u16 pitch)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundPitchID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 128, pitch);
    }
}

void SoundDopID(int soundId, u16 doppler)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundDopID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 132, doppler);
    }
}

void SoundModID(int soundId, u16 mod)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundModID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl14_with_irq_disabled(g_something_with_sound5[playerId][soundId], 1, mod);
    }
}

void SoundRevID(int soundId, u8 reverb)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundRevID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 91, reverb);
    }
}

void SoundChoID(int soundId, u8 chorus)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundChoID", soundId);
    if (g_something_with_sound5[playerId][soundId] != -1U)
    {
        g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[playerId][soundId], 93, chorus);
    }
}

BOOL SoundCheckStateID(int soundId)
{
    int playerId;

    soundId = search_sound_by_cue(soundId);
    playerId = get_sound_player_id("SoundCheckStateID", soundId);
    return g_something_with_sound5[playerId][soundId] != -1U;
}

/* Set the default reverb and chorus levels (-1 leaves one unchanged). */
void fn_1_32338(s8 reverb, s8 chorus)
{
    if (reverb != -1)
        lbl_1_bss_160A4 = reverb;
    if (chorus != -1)
        lbl_1_bss_160A5 = chorus;
}

/* ICS ("interactive continuous sound") sounds: three layered voices whose
 * volumes follow curves of an intensity parameter, with doppler pitch. */
struct IcsVoice
{
    u32 vid;
    s32 soundId;
};

struct IcsVoice lbl_1_data_23E00[8][3] =
{
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
    { { -1, 82 }, { -1, 83 }, { -1, 84 } },
};

struct Keyframe lbl_1_data_23EC0[] = { { 0, 0.0f, 0.0f, 0.0f }, { 127, 127.0f, 0.0f, 0.0f } };
struct Keyframe lbl_1_data_23EE0[] = { { 0, 127.0f, 0.0f, 0.0f }, { 127, 127.0f, 0.0f, 0.0f } };
struct Keyframe lbl_1_data_23F00[] = { { 0, 0.0f, 0.0f, 0.0f }, { 127, 127.0f, 0.0f, 0.0f } };
struct Keyframe *lbl_1_data_23F20[] = { lbl_1_data_23EC0, lbl_1_data_23EE0, lbl_1_data_23F00 };
struct Keyframe lbl_1_data_23F2C[] = { { 0, 4608.0f, 0.0f, 0.0f }, { 127, 15615.0f, 0.0f, 0.0f } };

void SoundIcsReq(u32 soundId, u8 intensity, s8 volume)
{
    SND_PARAMETER param;
    SND_PARAMETER_INFO info;
    s8 pan;
    s8 span;
    int playerId;
    struct { s32 active; s32 unk4; } *state;
    float scale;
    int i;
    struct IcsVoice *voice;
    s16 pitch;
    float diff;

    span = soundId >> 25;
    pan = (soundId >> 18) & 0x7F;
    soundId &= 0x7FF;
    if (pan > 63)
        pan = -(128 - pan);
    if (span > 63)
        span = -(128 - span);
    playerId = get_sound_player_id("SoundIcsReq", lbl_1_data_23E00[soundId][0].soundId);
    volume = (volume * 1.5f < 127.0f) ? volume * 1.5f : 127.0f;
    state = (void *)&lbl_1_bss_217AC[soundId];

    if (state->active == 0 && events[19].state == EV_STATE_RUNNING)
    {
        if (volume == 0 || intensity == 0)
            return;
        if (modeCtrl.unk30 > 0 && sub_mode != 0x79)
        {
            pan = get_ball_pan(playerId);
            span = get_ball_span(playerId);
            volume = volume * get_ball_volume(playerId);
        }
        param.ctrl = 0x83;
        param.paraData.value7 = 127 - (u8)(span + 0x40);
        info.numPara = 1;
        info.paraArray = &param;
        scale = volume / 127.0f;
        for (i = 0; i < 3; i++)
        {
            voice = &lbl_1_data_23E00[soundId][i];
            volume = scale * fn_1_D2550(intensity, lbl_1_data_23F20[i]);
            voice->vid = sndFXStartParaInfo(lbl_1_rodata_148C[voice->soundId].fxId, 127, pan + 0x40, 0, &info);
            g_something_with_sound5[soundId][voice->soundId] = voice->vid;
            g_call_synthFXSetCtrl_with_irq_disabled(voice->vid, 7, volume);
            lbl_1_bss_21B90[soundId][voice->soundId] = volume;
        }
        state->active = 1;
        lbl_1_bss_24F38[soundId] = get_ball_volume(playerId);
        return;
    }

    if (volume == 0 || intensity == 0)
    {
        for (i = 0; i < 3; i++)
        {
            voice = &lbl_1_data_23E00[soundId][i];
            sndFXKeyOff(voice->vid);
            lbl_1_bss_21B90[soundId][voice->soundId] = 0;
        }
        state->active = 0;
        return;
    }

    scale = volume / 127.0f;
    pitch = fn_1_D2550(intensity, lbl_1_data_23F2C);
    for (i = 0; i < 3; i++)
    {
        voice = &lbl_1_data_23E00[soundId][i];
        volume = scale * fn_1_D2550(intensity, lbl_1_data_23F20[i]);
        if (modeCtrl.unk30 > 0 && sub_mode != 0x79)
        {
            pan = (pan + get_ball_pan(playerId) < -64) ? -64
                : (pan + get_ball_pan(playerId) > 63) ? 63
                : pan + get_ball_pan(playerId);
            span = (span + get_ball_span(playerId) < -64) ? -64
                 : (span + get_ball_span(playerId) > 63) ? 63
                 : span + get_ball_span(playerId);
            volume = volume * get_ball_volume(playerId);
        }
        g_call_synthFXSetCtrl_with_irq_disabled(voice->vid, 7, volume);
        g_call_synthFXSetCtrl_with_irq_disabled(voice->vid, 10, pan + 0x40);
        g_call_synthFXSetCtrl_with_irq_disabled(voice->vid, 0x83, 0x7F - (span + 0x40));
        diff = get_ball_volume(playerId) - lbl_1_bss_24F38[soundId];
        if (diff < 0.0f)
            pitch = pitch - 768.0f * (intensity / 127.0f);
        else if (diff > 0.0f)
            pitch = pitch + 768.0f * (intensity / 127.0f);
        g_call_synthFXSetCtrl14_with_irq_disabled(voice->vid, 0x84, pitch);
        lbl_1_bss_24F38[soundId] = get_ball_volume(playerId);
        lbl_1_bss_21B90[soundId][voice->soundId] = volume;
    }
}

s16 fn_1_32DC8(void)
{
    s16 theme = (world_theme < 0) ? 0 : (world_theme > 42) ? 42 : world_theme;

#ifdef __MWERKS__
    return g_bgm_id_lookup_table[theme];
#else
    /* The GC clamps to 42 but the table has 42 entries (0..41), so theme 42
     * reads the first halfword of the next .rodata object (lbl_1_rodata_1284):
     * the high half of the relocated pointer to "GRPse04", a 0x80xx address,
     * i.e. a large negative track id that SoftStreamStart would use to index
     * SEL_WORLD_PTR far out of bounds. No stage has theme 42 (the highest in
     * STAGE_WORLD_THEMES is 41 and every stage select stops at 420), so it
     * takes an invalid stage id to get here; give such themes no BGM (-1),
     * like the other bg-less themes, instead of reading past the table. */
    if (theme >= (s16)(sizeof(g_bgm_id_lookup_table) / sizeof(g_bgm_id_lookup_table[0])))
        return -1;
    return g_bgm_id_lookup_table[theme];
#endif
}

/* Music request: the low 16 bits pick the track, the high 16 bits are a parameter. */
void g_fade_track_volume(u32 track, s8 cmd)
{
    int param;
    int i;
    int count;

    param = track >> 16;
    track &= 0xFFFF;
    if (cmd == 3 || cmd == 2)
    {
        param = track;
        track = g_active_music_tracks[fn_1_33368()];
    }
    else if (cmd == 8 || cmd == 10)
    {
        param = track;
        if (param == 0)
            param = 1;
        track = g_active_music_tracks[fn_1_33368()];
    }
    if (track == -1)
        track = g_active_music_tracks[fn_1_33368()];
    track %= 147;
    while ((SEL_WORLD_PTR[track].unk10 & 8) && track != 0)
        track--;

    switch (cmd)
    {
    case 0:
        count = 0;
        for (i = 0; i < 10; i++)
        {
            if (g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0)
                count++;
        }
        if (g_maybe_related_to_music_crossfading(track) >= 0)
            g_change_music_volume(track, 30, 100);
        else if (count > 0)
            play_track_and_fade_out_other_tracks(track, 30, 100);
        else
            g_SoftStreamStart_with_some_defaults_2(track);
        break;
    case 1:
        g_something_with_stopping_music_or_sfx(track);
        break;
    case 2:
        g_change_music_volume(track, param, 0);
        break;
    case 3:
        g_change_music_volume(track, param, 100);
        break;
    case 8:
        for (i = 0; i < 10; i++)
        {
            if (g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0
             && !(SEL_WORLD_PTR[g_active_music_tracks[i]].unk10 & 4))
                g_change_music_volume(g_active_music_tracks[i], 60, param);
        }
        break;
    case 10:
        lbl_1_bss_16038 = 0.01f * param;
        break;
    }
}

/* Find the port playing a track, or a free one. With outPort, also find a second
 * free port (or the paired one for mode 1) for the right channel. */
s16 get_smgr_port(s8 type, int track, s8 mode, s16 *outPort)
{
    s16 i;

    for (i = 0; i < 10; i++)
    {
        if (g_something_related_to_bgm_track_id[i] == type && track == g_active_music_tracks[i])
            break;
    }
    if (i == 10)
    {
        if (mode != 0)
            return -1;
        for (i = 0; i < 10; i++)
        {
            if (g_something_related_to_bgm_track_id[i] == -1 && fn_1_384EC(lbl_1_bss_15F30[i]) == 0
             && g_active_music_tracks[i] == -1)
                break;
        }
        if (i == 10)
        {
            g_printf_sound_error(0, "get_smgr_port\n", "%s, port is FULL.\n",
                                 (type == 0) ? fn_1_36488(track) : fn_1_34AB0(track));
            return -1;
        }
    }
    if (outPort != NULL)
    {
        if (mode == 1)
            *outPort = lbl_1_bss_24F58[i];
        else
        {
            for (*outPort = 0; *outPort < 10; (*outPort)++)
            {
                if (g_something_related_to_bgm_track_id[*outPort] == -1 && i != *outPort
                 && fn_1_384EC(lbl_1_bss_15F30[*outPort]) == 0 && g_active_music_tracks[*outPort] == -1)
                    break;
            }
            if (*outPort == 10)
            {
                g_printf_sound_error(0, "get_smgr_port2\n", "%s, port is FULL.\n",
                                     (type == 0) ? fn_1_36488(track) : fn_1_34AB0(track));
                return -1;
            }
        }
    }
    return i;
}

/* Index of the loudest playing music port. */
int fn_1_33368(void)
{
    int best = 0;
    int i;
    int maxVol = -1;

    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0
         && !(SEL_WORLD_PTR[g_active_music_tracks[i]].unk10 & 4) && lbl_1_bss_15FA0[i].vol2 >= maxVol)
        {
            maxVol = lbl_1_bss_15FA0[i].vol2;
            best = i;
        }
    }
    return best;
}

/* Free ports whose streams have finished. */
void fn_1_33418(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] < 0)
            continue;
        if (lbl_1_bss_16024[i] == 3 && lbl_1_bss_15F30[i] != -2U && fn_1_384EC(lbl_1_bss_15F30[i]) != 0)
            lbl_1_bss_16024[i] = 0;
        if ((g_something_related_to_bgm_track_id[i] == 0 || g_something_related_to_bgm_track_id[i] == 1)
         && fn_1_384EC(lbl_1_bss_15F30[i]) == 0)
            fn_1_3350C(i);
    }
}

/* Stop the stream on a port and release it (and its paired port, for music). */
void fn_1_3350C(s16 port)
{
    if (lbl_1_bss_16024[port] != 0)
        return;
    if (g_something_related_to_bgm_track_id[port] == 0 && lbl_1_bss_16024[lbl_1_bss_24F58[port]] != 0)
        return;
    fn_1_38220(lbl_1_bss_15F30[port]);
    lbl_1_bss_15F30[port] = -1;
    if (g_something_related_to_bgm_track_id[port] == 0)
    {
        lbl_1_bss_15FA0[port].vol2 = 100;
        lbl_1_bss_15FA0[port].duration = 0;
        g_active_music_tracks[lbl_1_bss_24F58[port]] = -1;
        g_something_related_to_bgm_track_id[lbl_1_bss_24F58[port]] = -1;
        lbl_1_bss_15F78[lbl_1_bss_24F58[port]] = 0;
    }
    g_active_music_tracks[port] = -1;
    g_something_related_to_bgm_track_id[port] = -1;
    lbl_1_bss_15F78[port] = 0;
    lbl_1_bss_24F58[port] = 0;
}

/* Current music volume scale, from the loudest playing music track. */
void g_something_with_bgm(void)
{
    int i;
    int maxVol = -1;

    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] != -1
         && (g_something_related_to_bgm_track_id[i] == 0 || g_something_related_to_bgm_track_id[i] == 3)
         && lbl_1_bss_15FA0[i].vol2 >= maxVol)
            maxVol = lbl_1_bss_15FA0[i].vol2;
    }
    lbl_1_bss_16034 = lbl_1_bss_16030;
    lbl_1_bss_16030 = lbl_1_bss_16038 * (0.01f * ((maxVol == -1) ? 100 : maxVol));
}

static inline void set_port_volume(s16 port, s8 volume)
{
    if (lbl_1_bss_24F64 != 0)
        volume = 0;
    volume = 100.0f * math_sin((volume * 164 < 0x4000) ? volume * 164 : 0x4000);
    fn_1_38618(lbl_1_bss_24F68[port], volume);
    fn_1_38618(lbl_1_bss_24F68[lbl_1_bss_24F58[port]], volume);
}

/* Advance music volume fades and apply volumes to the streams. */
void g_crossfade_music(void)
{
    int i;
    struct SoundTrackVolume *fade;
    struct SoundTrackVolume *track;
    struct SoundFileName *sfx;

    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] == -1)
            continue;
        if (g_something_related_to_bgm_track_id[i] != 0 && g_something_related_to_bgm_track_id[i] != 3)
            continue;
        if (g_something_related_to_bgm_track_id[i] == 0 && lbl_1_bss_15F30[i] == -2U)
            continue;
        if (fn_1_38A74(i) != 0)
            continue;
        fade = &lbl_1_bss_15FA0[i];
        if (fade->duration == 0)
            continue;
        if (!(g_some_other_flags & 0xA) || (g_some_other_flags & 8))
            fade->timer++;
        fade->vol2 = fade->vol1 + fade->timer * ((float)fade->delta / fade->duration);
        if (g_something_related_to_bgm_track_id[i] != 3)
            set_port_volume(i, fade->vol2);
        if (fade->timer == fade->duration)
        {
            fade->duration = fade->timer = 0;
            fade->vol1 = fade->vol2;
            if (g_something_related_to_bgm_track_id[i] == 3)
            {
                g_active_music_tracks[i] = -1;
                g_something_related_to_bgm_track_id[i] = -1;
            }
            else if (fade->vol1 <= 0)
                g_something_with_stopping_music_or_sfx(g_active_music_tracks[i]);
        }
    }
    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0)
        {
            track = &lbl_1_bss_15FA0[i];
            set_port_volume(i, track->vol2 * lbl_1_bss_16038);
        }
    }
    lbl_1_bss_1603C = lbl_1_bss_16038;
    g_something_with_bgm();
    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] != -1 && g_something_related_to_bgm_track_id[i] == 1)
        {
            sfx = &lbl_1_data_20E88[g_active_music_tracks[i]];
            if (!(sfx->flags & 1))
                fn_1_3874C(lbl_1_bss_24F68[i], sfx->volume * lbl_1_bss_16030);
        }
    }
}

#ifndef __MWERKS__
/* PC port: DSP headers are read raw from disc (big-endian); swap them in place. */
static void pc_swap_dsp_header(struct DSPHeader *h)
{
    u32 *w[] = { &h->numSamples, &h->numNibbles, &h->sampleRate, &h->loopStart, &h->loopEnd, &h->ca };
    int k;

    for (k = 0; k < 6; k++)
        *w[k] = __builtin_bswap32(*w[k]);
    h->loopFlag = __builtin_bswap16(h->loopFlag);
    h->format = __builtin_bswap16(h->format);
    for (k = 0; k < 16; k++)
        h->coefs[k] = __builtin_bswap16(h->coefs[k]);
}
#endif

/* DVD read callback for streamed sound effects. */
void fn_1_33DC0(s32 result, void *fileInfo)
{
    int i;

    if (DVDGetCommandBlockStatus(fileInfo) != 0)
        return;
    for (i = 0; i < 10; i++)
    {
        if ((void *)&lbl_1_bss_24F74[i].fileInfo == fileInfo)
            break;
    }
#ifndef __MWERKS__
    if (i < 10)
        pc_swap_dsp_header(&lbl_1_bss_25300[i]);
#endif
    lbl_1_bss_16024[i] = 2;
    g_some_music_related_counter[i] = 2;
    file_close(&lbl_1_bss_24F74[i]);
}

/* Start streamed sound effects whose delay has run out. */
void fn_1_33E64(void)
{
    int i;
    struct StreamSEReq *req;

    for (i = 0; i < 10; i++)
    {
        if (g_something_related_to_bgm_track_id[i] == 1 && g_some_music_related_counter[i] != -1)
        {
            if (g_some_music_related_counter[i] > 0)
                g_some_music_related_counter[i]--;
            if (g_some_music_related_counter[i] == 0 && lbl_1_bss_16024[i] == 2)
            {
                req = &lbl_1_bss_25294[i];
                SoftStreamSEReq(2, req->id, req->volume, req->pan, req->span);
                lbl_1_bss_16024[i] = 3;
                g_some_music_related_counter[i] = -1;
            }
        }
    }
}

void SoftStreamSEReq(s8 mode, int id, s8 volume, s8 pan, s8 span)
{
    struct SoundFileName *sfx;
    s16 port;
    char fileName[0x40];
    s8 channel;
    u32 stream;
    struct StreamParams *params;
    struct DSPHeader *header;

    sfx = &lbl_1_data_20E88[id];
#ifndef __MWERKS__
    printf("[snd] SoftStreamSEReq mode %d id %d (%s) vol %d\n", mode, id, sfx->file, volume);
#endif
    if (mode == 1)
        return;
    if (sfx->flags & 4)
        return;
    if (sfx->flags & 2)
    {
        SoftStreamStart(16, id + 147, volume);
        return;
    }
    port = get_smgr_port(1, id, 1, NULL);
    if (mode != 0 && port == -1)
        return;
    if (port == -1 || g_active_music_tracks[port] == -1 || mode == 2)
    {
        sprintf(fileName, "snd/stream_se/%s.dsp", sfx->file);
        if (mode == 0)
        {
            stream = fn_1_378D4(NULL, volume, pan + 0x40, 0x7F - (span + 0x40), &channel, -1);
            if (stream == -1U)
            {
                g_printf_sound_error(0, "SoftStreamSEReq\n", "%s can't start / PORT FULL!\n", fileName);
                return;
            }
            port = channel;
            lbl_1_bss_24F68[port] = channel;
            lbl_1_bss_252D0[port] = stream;
            file_close(&lbl_1_bss_24F74[port]);
            if (!file_open(fileName, &lbl_1_bss_24F74[port]))
            {
                g_printf_sound_error(0, "SoftStreamSEReq\n", "Can't find %s\n", fileName);
                return;
            }
            DVDReadAsyncPrio(&lbl_1_bss_24F74[port].fileInfo, &lbl_1_bss_25300[port], 0x60, 0, fn_1_33DC0, 2);
            lbl_1_bss_25294[port].id = id;
            lbl_1_bss_25294[port].volume = volume;
            lbl_1_bss_25294[port].pan = pan;
            lbl_1_bss_25294[port].span = span;
            lbl_1_bss_16024[port] = 1;
            lbl_1_bss_15F30[port] = -2;
            g_active_music_tracks[port] = id;
            g_something_related_to_bgm_track_id[port] = 1;
            return;
        }
        if (mode == 2)
            header = &lbl_1_bss_25300[port];
        else
            return;
        params = &lbl_1_bss_256C0[port];
        memcpy_handler(params->coefs, header->coefs, sizeof(params->coefs));
        params->fileName = fileName;
        params->sampleRate = header->sampleRate;
        params->loopEnd = (header->loopEnd >> 1) & ~31;
        params->dataOffset = 0x60;
        params->loopFlag = header->loopFlag;
        params->loopStart = (header->loopStart >> 1) & ~31;
        params->frameRate = 60.0f;
        fn_1_378D4(params, volume, pan + 0x40, 0x7F - (span + 0x40), &lbl_1_bss_24F68[port], 0);
        lbl_1_bss_15F30[port] = lbl_1_bss_252D0[port];
        if (lbl_1_bss_15F30[port] == -1U)
        {
            g_printf_sound_error(0, "SoftStreamSEReq\n", "%s can't start. port is full.\n", fn_1_34AB0(id));
            return;
        }
        sfx->volume = volume;
        if (dip_switches & DIP_SNDREQ_DISP)
            printf("STRMSE[%-35s] vol:%3d(%3d) pan:%3d span:%3d\n", fn_1_34AB0(id), volume, volume, pan, span);
    }
    else if (lbl_1_bss_16024[port] != 1 && lbl_1_bss_16024[port] != 2 && fn_1_34A28(id) != -1)
    {
        g_stop_music_sound_debug(id);
        call_SoftStreamSEReq_arg_0(id, volume, pan, span);
    }
}

void fn_1_343F4(void) {}

void fn_1_343F8(int id)
{
    SoftStreamSEReq(1, id, 0, 0, 0);
}

/* Pause a streamed sound effect. */
void fn_1_3442C(int id)
{
    struct SoundFileName *sfx = &lbl_1_data_20E88[id];
    s16 port;
    s16 port2;

    if (sfx->flags & 4)
        return;
    if (sfx->flags & 2)
    {
        port = get_smgr_port(0, id + 147, 1, &port2);
        if (port != -1 && port2 != -1)
        {
            fn_1_389AC(port, 1);
            fn_1_389AC(port2, 1);
        }
    }
    else
    {
        port = get_smgr_port(1, id, 1, NULL);
        if (port != -1)
            fn_1_389AC(port, 1);
    }
}

/* Resume a streamed sound effect. */
void fn_1_344EC(int id)
{
    struct SoundFileName *sfx = &lbl_1_data_20E88[id];
    s16 port;
    s16 port2;

    if (sfx->flags & 4)
        return;
    if (sfx->flags & 2)
    {
        port = get_smgr_port(0, id + 147, 1, &port2);
        if (port != -1 && port2 != -1)
        {
            fn_1_389AC(port, 0);
            fn_1_389AC(port2, 0);
        }
    }
    else
    {
        port = get_smgr_port(1, id, 1, NULL);
        if (port != -1)
            fn_1_389AC(port, 0);
    }
}

void call_SoftStreamSEReq_arg_0(int id, s8 volume, s8 pan, s8 span)
{
    SoftStreamSEReq(0, id, volume, pan, span);
}

void g_stop_music_sound_debug(int id)
{
    struct SoundFileName *sfx = &lbl_1_data_20E88[id];
    s16 port;

    if (sfx->flags & 4)
        return;
    if (sfx->flags & 2)
    {
        g_something_with_stopping_music_or_sfx(id + 147);
        return;
    }
    port = get_smgr_port(1, id, 1, NULL);
    if (port != -1 && fn_1_384EC(lbl_1_bss_15F30[port]) == 1)
        fn_1_3350C(port);
}

/* Stop all streamed sound effects. */
void fn_1_34828(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (g_something_related_to_bgm_track_id[i] == 1 && fn_1_384EC(lbl_1_bss_15F30[i]) == 1)
            fn_1_3350C(i);
    }
}

/* Returns the port playing a streamed sound effect, or -1 if it isn't playing. */
s16 fn_1_34A28(int id)
{
    s16 port = get_smgr_port(1, id, 1, NULL);

    if (port == -1)
        return port;
    if ((fn_1_38944(port) == 0 && fn_1_3892C(port) == 0) || fn_1_38974(port) == 0)
        port = -1;
    return port;
}

/* Note: these return a pointer to a stack buffer, as in the original. */
char *fn_1_34AB0(int id)
{
#ifndef __MWERKS__
    static /* GCC turns the dangling return into NULL */
#endif
    char name[0x80];

    sprintf(name, "SOFT_STRM_SE_%s", lbl_1_data_20E88[id].name);
    return name;
}

char *fn_1_34AFC(int id)
{
#ifndef __MWERKS__
    static
#endif
    char name[0x80];

    sprintf(name, "%s", lbl_1_data_20E88[id].name);
    return name;
}

/* Set the volume of a streamed sound effect. */
void fn_1_34B48(int id, s8 volume)
{
    struct SoundFileName *sfx = &lbl_1_data_20E88[id];
    s16 port;

    if (sfx->flags & 4)
        return;
    if (sfx->flags & 2)
    {
        if (id == 0)
            g_maybe_smth_with_music(id + 147, volume);
        return;
    }
    port = get_smgr_port(1, id, 1, NULL);
    if (port != -1)
    {
        fn_1_3874C(lbl_1_bss_24F68[port], volume);
        sfx->volume = volume;
    }
}

/* Set the pan of a streamed sound effect. */
void fn_1_34C08(int id, s8 pan, s8 span)
{
    s16 port = get_smgr_port(1, id, 1, NULL);

    if (port != -1)
        fn_1_387E0(lbl_1_bss_24F68[port], pan + 0x40, 0x7F - (span + 0x40));
}

/* DVD read callback for streamed music. */
void g_some_dvd_read_async_sound_callback2(s32 result, void *fileInfo)
{
    int i;

    if (DVDGetCommandBlockStatus(fileInfo) != 0)
        return;
    for (i = 0; i < 10; i++)
    {
        if ((void *)&lbl_1_bss_24F74[i].fileInfo == fileInfo)
            break;
    }
#ifndef __MWERKS__
    if (i < 10)
        pc_swap_dsp_header(&lbl_1_bss_25300[i]);
#endif
    lbl_1_bss_16024[i] = 2;
    g_some_music_related_counter[i] = 2;
    file_close(&lbl_1_bss_24F74[i]);
}

/* Start streamed music once both channels' headers have been read. */
void fn_1_34D34(void)
{
    int i;
    int pair;
    struct StreamSEReq *req;

    for (i = 0; i < 10; i++)
    {
        if (g_something_related_to_bgm_track_id[i] != 0 && g_something_related_to_bgm_track_id[i] != 2)
            continue;
        pair = lbl_1_bss_24F58[i];
        if (g_something_related_to_bgm_track_id[i] == 0)
        {
            if (g_some_music_related_counter[i] == -1)
                continue;
            if (g_some_music_related_counter[i] > 0)
                g_some_music_related_counter[i]--;
        }
        else
        {
            if (g_some_music_related_counter[i] != 0)
                g_some_music_related_counter[i]--;
            continue;
        }
        if (g_some_music_related_counter[i] == 0 && lbl_1_bss_16024[i] == 2 && lbl_1_bss_16024[pair] == 2
         && g_some_music_related_counter[pair] == 0)
        {
            req = &lbl_1_bss_25294[i];
            if (lbl_1_bss_15F78[i] != 0)
            {
                if (SoftStreamStart(18, req->id, req->volume) == -1)
                    continue;
            }
            else
            {
                if (SoftStreamStart(2, req->id, req->volume) == -1)
                    continue;
            }
            lbl_1_bss_16024[i] = 3;
            lbl_1_bss_16024[pair] = 3;
            g_some_music_related_counter[i] = -1;
            g_some_music_related_counter[pair] = -1;
        }
    }
}

/* Start (flags & 0xF == 0: open files and read headers; 2: start playback) a stereo
 * music stream. With flags & 0x10, the track is a pair of streamed sound effects. */
s16 SoftStreamStart(u8 flags, int track, s8 volume)
{
    struct File file;
    char pathL[0x40];
    char pathR[0x40];
    s16 port2;
    s8 channels[2];
    s16 port;
    struct StreamParams *params;
    int i;
    struct DSPHeader *headerL;
    struct DSPHeader *headerR;
    s8 vol;
    u32 stream;
    struct DSPHeader *header;
    struct SelWorldSound *world;
    struct SoundFileName *sfxR;
    struct SoundFileName *sfxL;
    float frameRate;
    s8 idx;

#ifndef __MWERKS__
    if ((flags & 0xF) != 1)
        printf("[snd] SoftStreamStart flags 0x%02x track %d vol %d\n", flags, track, volume);
#endif
    if ((flags & 0xF) == 1)
        return -1;
    port = get_smgr_port(0, track, 1, &port2);
    if ((flags & 0xF) != 0)
    {
        if (port == -1)
            return -1;
        if (port2 == -1)
            return -1;
    }
    if (port == -1 || g_active_music_tracks[port] == -1 || (flags & 0xF) == 2)
    {
        if (flags & 0x10)
        {
            sfxL = &lbl_1_data_20E88[track - 147];
            sfxR = &lbl_1_data_20E88[track - 146];
            sprintf(pathL, "snd/stream_se/%s.dsp", sfxL->file);
            sprintf(pathR, "snd/stream_se/%s.dsp", sfxR->file);
            vol = volume;
            frameRate = 60.0f;
        }
        else
        {
            world = &SEL_WORLD_PTR[track];
            if (world->unk10 & 2)
            {
                sprintf(pathL, "snd/stream/%s.dsp", world->file);
                sprintf(pathR, "snd/stream/%s.dsp", world->file);
            }
            else
            {
                sprintf(pathL, "snd/stream/%s_L.dsp", world->file);
                sprintf(pathR, "snd/stream/%s_R.dsp", world->file);
            }
            vol = world->volume;
            frameRate = world->frameRate;
        }
        if (lbl_1_bss_24F64 != 0)
            vol = 0;

        if ((flags & 0xF) == 0)
        {
            if (!file_open(pathL, &file))
            {
                g_printf_sound_error(0, "SoftStreamStart\n", "Can't find %s\n", pathL);
                return -1;
            }
            file_close(&file);
            if (!file_open(pathR, &file))
            {
                g_printf_sound_error(0, "SoftStreamStart\n", "Can't find %s\n", pathR);
                return -1;
            }
            file_close(&file);
            if (flags & 0x10)
                stream = g_smgr_allocate_stereo(NULL, NULL, vol, volume, &channels[0], &channels[1], -1);
            else
                stream = g_smgr_allocate_stereo(NULL, NULL, vol + lbl_1_bss_25918[track], volume, &channels[0], &channels[1], -1);
            if (stream == -1U)
            {
                g_printf_sound_error(0, "SoftStreamStart\n", "%s can't start / PORT FULL!\n", pathL);
                return -1;
            }
            port = channels[0];
            port2 = channels[1];
            file_close(&lbl_1_bss_24F74[port]);
            file_open(pathL, &lbl_1_bss_24F74[port]);
            DVDReadAsyncPrio(&lbl_1_bss_24F74[port].fileInfo, &lbl_1_bss_25300[port], 0x60, 0,
                             g_some_dvd_read_async_sound_callback2, 2);
            file_close(&lbl_1_bss_24F74[port2]);
            file_open(pathR, &lbl_1_bss_24F74[port2]);
            DVDReadAsyncPrio(&lbl_1_bss_24F74[port2].fileInfo, &lbl_1_bss_25300[port2], 0x60, 0,
                             g_some_dvd_read_async_sound_callback2, 2);
            lbl_1_bss_24F68[port] = channels[0];
            lbl_1_bss_24F68[port2] = channels[1];
            lbl_1_bss_252D0[port] = stream;
            lbl_1_bss_25294[port].id = track;
            lbl_1_bss_25294[port].volume = volume;
            lbl_1_bss_25294[port].pan = 0;
            lbl_1_bss_25294[port].span = 0;
            lbl_1_bss_25294[port2] = lbl_1_bss_25294[port];
            lbl_1_bss_16024[port] = 1;
            lbl_1_bss_16024[port2] = 1;
            lbl_1_bss_15F30[port] = -2;
            lbl_1_bss_15F30[port2] = -2;
            g_active_music_tracks[port] = track;
            g_something_related_to_bgm_track_id[port] = 0;
            lbl_1_bss_24F58[port] = port2;
            g_active_music_tracks[port2] = track;
            g_something_related_to_bgm_track_id[port2] = 2;
            if (flags & 0x10)
            {
                lbl_1_bss_15F78[port] = 1;
                lbl_1_bss_15F78[port2] = 1;
            }
            else
            {
                lbl_1_bss_15F78[port] = 0;
                lbl_1_bss_15F78[port2] = 0;
            }
            lbl_1_bss_15FA0[port].duration = 0;
            lbl_1_bss_15FA0[port].timer = 0;
            lbl_1_bss_15FA0[port].vol1 = volume;
            lbl_1_bss_15FA0[port].vol2 = volume;
            lbl_1_bss_15FA0[port].delta = 0;
            g_something_with_bgm();
            return port;
        }

        if ((flags & 0xF) == 2)
        {
            headerL = &lbl_1_bss_25300[port];
            headerR = &lbl_1_bss_25300[port2];
        }
        else
            return -1;
        for (i = 0; i < 2; i++)
        {
            idx = (i == 0) ? port : port2;
            header = (i == 0) ? headerL : headerR;
            params = &lbl_1_bss_256C0[idx];
            memcpy_handler(params->coefs, header->coefs, sizeof(params->coefs));
            params->fileName = ((i & 1) == 0) ? pathL : pathR;
            params->sampleRate = header->sampleRate;
            params->loopEnd = (header->loopEnd >> 1) & ~31;
            params->dataOffset = 0x60;
            params->loopFlag = header->loopFlag;
            params->loopStart = (header->loopStart >> 1) & ~31;
            params->frameRate = frameRate;
        }
        if (flags & 0x10)
            g_smgr_allocate_stereo(&lbl_1_bss_256C0[port], &lbl_1_bss_256C0[port2], vol, volume,
                                   &lbl_1_bss_24F68[port], &lbl_1_bss_24F68[port2], 0);
        else
            g_smgr_allocate_stereo(&lbl_1_bss_256C0[port], &lbl_1_bss_256C0[port2], vol + lbl_1_bss_25918[track],
                                   volume, &lbl_1_bss_24F68[port], &lbl_1_bss_24F68[port2], 0);
        lbl_1_bss_15F30[port] = lbl_1_bss_252D0[port];
        lbl_1_bss_15F30[port2] = lbl_1_bss_252D0[port];
        if (lbl_1_bss_15F30[port] == -1U)
        {
            g_printf_sound_error(0, "SoftStreamStart\n", "%s can't start. port is full.\n", fn_1_36488(track));
            return -1;
        }
    }
    else if (!(flags & 0x10) && lbl_1_bss_16024[port] != 1 && lbl_1_bss_16024[port] != 2
          && lbl_1_bss_16024[port2] != 1 && lbl_1_bss_16024[port2] != 2
          && g_maybe_related_to_music_crossfading(track) != -1)
    {
        g_something_with_stopping_music_or_sfx(track);
        port = SoftStreamStart(0, track, volume);
    }
    return port;
}

void fn_1_358E8(void) {}

s16 g_SoftStreamStart_with_some_defaults(int track)
{
    return SoftStreamStart(1, track, 0);
}

/* Pause streamed music. */
void fn_1_35918(int track)
{
    s16 port2;
    s16 port = get_smgr_port(0, track, 1, &port2);

    if (port != -1 && port2 != -1)
    {
        fn_1_389AC(port, 1);
        fn_1_389AC(port2, 1);
    }
}

/* Resume streamed music. */
void fn_1_35978(int track)
{
    s16 port2;
    s16 port = get_smgr_port(0, track, 1, &port2);

    if (port != -1 && port2 != -1)
    {
        fn_1_389AC(port, 0);
        fn_1_389AC(port2, 0);
    }
}

s16 g_SoftStreamStart_with_some_defaults_2(int track)
{
    return SoftStreamStart(0, track, 100);
}

s16 g_another_SoftStreamStart_wrapper_of_some_sort(int track, s8 volume)
{
    return SoftStreamStart(0, track, volume);
}

/* Start a track silently and fade it in, fading out all other music. */
s16 play_track_and_fade_out_other_tracks(int track, int fadeTime, s8 volume)
{
    s16 i;
    s16 port;

    port = SoftStreamStart(0, track, 0);
    if (port == -1)
        port = -1;
    else
    {
        g_change_music_volume(track, 15, volume);
        for (i = 0; i < 10; i++)
        {
            if (port != i && g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0
             && !(SEL_WORLD_PTR[g_active_music_tracks[i]].unk10 & 4))
                g_change_music_volume(g_active_music_tracks[i], fadeTime, 0);
        }
    }
    return port;
}

s16 g_smth_related_to_music(int track, int fadeTime, int otherFadeTime, s8 volume, s8 otherVolume)
{
    s16 i;
    s16 port;

    port = SoftStreamStart(0, track, 0);
    if (port == -1)
        port = -1;
    else
    {
        g_change_music_volume(track, fadeTime, volume);
        for (i = 0; i < 10; i++)
        {
            if (port != i && g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0
             && !(SEL_WORLD_PTR[g_active_music_tracks[i]].unk10 & 4))
                g_change_music_volume(g_active_music_tracks[i], otherFadeTime, otherVolume);
        }
    }
    return port;
}

/* Play the current world's music. */
void g_handle_world_bgm(s8 volume)
{
    s16 track = fn_1_32DC8();
    int count;
    int i;
    s16 port;
    s16 j;
    if (track == -1)
    {
        g_fade_track_volume(world_theme, 0);
        return;
    }
    if (g_maybe_related_to_music_crossfading(track) >= 0)
    {
        g_change_music_volume(-1, 120, 0);
        g_change_music_volume(track, 120, volume);
        return;
    }
    count = 0;
    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] >= 0 && g_something_related_to_bgm_track_id[i] == 0)
            count++;
    }
    if (g_maybe_related_to_music_crossfading(track) >= 0)
        g_change_music_volume(track, 30, 100);
    else if (count > 0)
    {
        port = SoftStreamStart(0, track, 0);
        if (port != -1)
        {
            g_change_music_volume(track, 15, volume);
            for (j = 0; j < 10; j++)
            {
                if (port != j && g_active_music_tracks[j] >= 0 && g_something_related_to_bgm_track_id[j] == 0
                 && !(SEL_WORLD_PTR[g_active_music_tracks[j]].unk10 & 4))
                    g_change_music_volume(g_active_music_tracks[j], 30, 0);
            }
        }
    }
    else
        SoftStreamStart(0, track, volume);
}

void g_something_with_stopping_music_or_sfx(int track)
{
    s16 port = get_smgr_port(0, track, 1, NULL);

    if (port != -1 && fn_1_384EC(lbl_1_bss_15F30[port]) == 1)
        fn_1_3350C(port);
}

/* Stop all music. */
void fn_1_36140(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (g_something_related_to_bgm_track_id[i] == 0 && fn_1_384EC(lbl_1_bss_15F30[i]) == 1)
            fn_1_3350C(i);
    }
}

/* Returns the port playing a music track, or -1 if it isn't playing. */
s16 g_maybe_related_to_music_crossfading(int track)
{
    s16 port = get_smgr_port(0, track, 1, NULL);

    if (port == -1)
        return port;
    if ((fn_1_38944(port) == 0 && fn_1_3892C(port) == 0) || fn_1_38974(port) == 0)
        port = -1;
    return port;
}

u32 fn_1_363C8(int track)
{
    s16 port = get_smgr_port(0, track, 1, NULL);

    if (port == -1)
        return 0;
    return fn_1_38944(lbl_1_bss_24F68[port]);
}

u32 g_check_current_track(int track)
{
    s16 port = get_smgr_port(0, track, 1, NULL);

    if (port == -1)
        return 0;
    return fn_1_3895C(lbl_1_bss_24F68[port]);
}

/* Note: these return a pointer to a stack buffer, as in the original. */
char *fn_1_36488(int track)
{
#ifndef __MWERKS__
    static
#endif
    char name[0x80];

    if (track >= 147)
        return "???";
    sprintf(name, "SOFT_STRM_%s", SEL_WORLD_PTR[track].name);
    return name;
}

char *fn_1_364E8(int track)
{
#ifndef __MWERKS__
    static
#endif
    char name[0x80];

    sprintf(name, "%s", SEL_WORLD_PTR[track].name);
    return name;
}

s8 fn_1_36534(int track)
{
    return SEL_WORLD_PTR[track].volume;
}

s8 fn_1_36550(int track)
{
    s16 port = get_smgr_port(0, track, 1, NULL);
    struct SoundTrackVolume *fade;

    if (port == -1)
        return 0;
    fade = &lbl_1_bss_15FA0[port];
    return fade->vol2;
}

void fn_1_365AC(int track, s8 volume)
{
    SEL_WORLD_PTR[track].volume = volume;
}

void g_maybe_smth_with_music(int track, s8 volume)
{
    g_change_music_volume(track, 1, volume);
}

void fn_1_365F0(int track, int value)
{
    s16 port = get_smgr_port(0, track, 1, NULL);

    if (port != -1)
        fn_1_38888(lbl_1_bss_24F68[port], value);
}

/* Fade a music track (or all music, for track -1) to a volume over time frames. */
void g_change_music_volume(int track, u32 time, s8 volume)
{
    s16 port;
    s16 i;
    s16 end;
    s16 started = 0;
    struct SoundTrackVolume *fade;

    if (track == -1)
    {
        port = 0;
        end = 10;
    }
    else
    {
        port = get_smgr_port(0, track, 1, NULL);
        if (fn_1_38A74(port) != 0)
            return;
        end = port + 1;
    }
    if (port == -1)
    {
        port = 0;
        end = 10;
    }
    if (time == 0)
        return;
    for (i = port; i < end; i++)
    {
        if (g_active_music_tracks[i] == -1)
        {
            if (started > 0)
                continue;
            started++;
            g_active_music_tracks[i] = -2;
            g_something_related_to_bgm_track_id[i] = 3;
            lbl_1_bss_15FA0[i].vol1 = 100;
        }
        else if (g_something_related_to_bgm_track_id[i] != 0 || fn_1_38A74(i) != 0)
            continue;
        fade = &lbl_1_bss_15FA0[i];
        fade->duration = time;
        fade->timer = 0;
        fade->vol1 = fade->vol2;
        fade->delta = volume - fade->vol2;
        port++;
    }
}

/* Set the console sound mode (mono/stereo), reapplying the music volumes. */
void fn_1_367E4(int mode)
{
    int i;
    struct SoundTrackVolume *fade;

    if (mode == OSGetSoundMode())
        return;
    if (mode == -1)
        mode = OSGetSoundMode();
    OSSetSoundMode(mode);
    if (mode == OS_SOUND_MODE_MONO)
        fn_8003DCDC(0);
    else
        fn_8003DCDC(2);
    for (i = 0; i < 10; i++)
    {
        if (g_active_music_tracks[i] < 0)
            continue;
        if (g_something_related_to_bgm_track_id[i] == 0)
        {
            fade = &lbl_1_bss_15FA0[i];
            set_port_volume(i, fade->vol2);
        }
        else if (g_something_related_to_bgm_track_id[i] == 1)
            fn_1_388A0(i);
    }
}

/* Set up the reverb and chorus aux effects, if there is enough memory. */
void SoundEffectInit(void)
{
    if (lbl_1_bss_259AC != 0)
        return;
    if (OSCheckHeap(main_heap) < 0x20000)
    {
        g_printf_sound_error(1, "SoundEffectInit\n",
                             "Sound effecter disable at this scene.\nUunable to allocate buffer.\n", 0);
        return;
    }
    sndAuxCallbackPrepareReverbHI(&lbl_1_bss_259B0);
    sndAuxCallbackPrepareChorus(&lbl_1_bss_25B90);
    sndSetAuxProcessingCallbacks(0, fn_8005205C, &lbl_1_bss_259B0, 0xFF, 0, fn_80053430,
                                 &lbl_1_bss_25B90, 0xFF, 0);
    lbl_1_bss_259AC = TRUE;
}

/* Shut down the aux effects. */
void g_something_with_sound9(void)
{
    if (lbl_1_bss_259AC != 0)
    {
        g_something_with_sound8_wrapper(&lbl_1_bss_259B0);
        sndAuxCallbackShutdownChorus(&lbl_1_bss_25B90);
        sndSetAuxProcessingCallbacks(0, NULL, &lbl_1_bss_259B0, 0xFF, 0, NULL, &lbl_1_bss_25B90, 0xFF, 0);
        lbl_1_bss_259AC = FALSE;
    }
}

void g_snd_dbg_call_sndAuxCallbackupdateSettingsReverbHI_1(struct ReverbSettings *settings)
{
    if (lbl_1_bss_259AC != 0)
    {
        lbl_1_bss_259B0.settings.coloration = settings->coloration;
        lbl_1_bss_259B0.settings.mix = settings->mix;
        lbl_1_bss_259B0.settings.time = settings->time;
        lbl_1_bss_259B0.settings.damping = settings->damping;
        lbl_1_bss_259B0.settings.preDelay = settings->preDelay;
        lbl_1_bss_259B0.settings.crosstalk = settings->crosstalk;
        sndAuxCallbackUpdateSettingsReverbHI(&lbl_1_bss_259B0);
    }
}

void g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_2(struct ChorusSettings *settings)
{
    if (lbl_1_bss_259AC != 0)
    {
        lbl_1_bss_25B90.settings.baseDelay = settings->baseDelay;
        lbl_1_bss_25B90.settings.variation = settings->variation;
        lbl_1_bss_25B90.settings.period = settings->period;
        sndAuxCallbackUpdateSettingsChorus(&lbl_1_bss_25B90);
    }
}

void g_snd_dbg_call_sndAuxCallbackUpdateSettingsReverbHI_2(void)
{
    if (lbl_1_bss_259AC != 0)
    {
        lbl_1_bss_259B0.settings.coloration = lbl_1_rodata_700C.coloration;
        lbl_1_bss_259B0.settings.mix = lbl_1_rodata_700C.mix;
        lbl_1_bss_259B0.settings.time = lbl_1_rodata_700C.time;
        lbl_1_bss_259B0.settings.damping = lbl_1_rodata_700C.damping;
        lbl_1_bss_259B0.settings.preDelay = lbl_1_rodata_700C.preDelay;
        lbl_1_bss_259B0.settings.crosstalk = lbl_1_rodata_700C.crosstalk;
        sndAuxCallbackUpdateSettingsReverbHI(&lbl_1_bss_259B0);
    }
}

void g_snd_dbg_call_sndAuxCallbackUpdateSettingsChorus_1(void)
{
    if (lbl_1_bss_259AC != 0)
    {
        lbl_1_bss_25B90.settings.baseDelay = lbl_1_rodata_7024.baseDelay;
        lbl_1_bss_25B90.settings.variation = lbl_1_rodata_7024.variation;
        lbl_1_bss_25B90.settings.period = lbl_1_rodata_7024.period;
        sndAuxCallbackUpdateSettingsChorus(&lbl_1_bss_25B90);
    }
}

void g_set_some_sound_data(void)
{
    lbl_1_bss_259B0.settings.coloration = lbl_1_rodata_700C.coloration;
    lbl_1_bss_259B0.settings.mix = lbl_1_rodata_700C.mix;
    lbl_1_bss_259B0.settings.time = lbl_1_rodata_700C.time;
    lbl_1_bss_259B0.settings.damping = lbl_1_rodata_700C.damping;
    lbl_1_bss_259B0.settings.preDelay = lbl_1_rodata_700C.preDelay;
    lbl_1_bss_259B0.settings.crosstalk = lbl_1_rodata_700C.crosstalk;
}

void fn_1_36DCC(void)
{
    lbl_1_bss_25B90.settings.baseDelay = lbl_1_rodata_7024.baseDelay;
    lbl_1_bss_25B90.settings.variation = lbl_1_rodata_7024.variation;
    lbl_1_bss_25B90.settings.period = lbl_1_rodata_7024.period;
}

void fn_1_36E18(struct ReverbSettings *settings)
{
    settings->coloration = lbl_1_bss_259B0.settings.coloration;
    settings->mix = lbl_1_bss_259B0.settings.mix;
    settings->time = lbl_1_bss_259B0.settings.time;
    settings->damping = lbl_1_bss_259B0.settings.damping;
    settings->preDelay = lbl_1_bss_259B0.settings.preDelay;
    settings->crosstalk = lbl_1_bss_259B0.settings.crosstalk;
}

void fn_1_36E7C(struct ChorusSettings *settings)
{
    settings->baseDelay = lbl_1_bss_25B90.settings.baseDelay;
    settings->variation = lbl_1_bss_25B90.settings.variation;
    settings->period = lbl_1_bss_25B90.settings.period;
}

/* Mute all playing sound effects. */
void g_something_with_sound6(void)
{
    int j;
    int i;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 0x5B8; j++)
        {
            if (g_something_with_sound5[i][j] != -1U)
                g_call_synthFXSetCtrl_with_irq_disabled(g_something_with_sound5[i][j], 7, 0);
        }
    }
}

/* Reapply the volume of every playing sound effect. */
void g_something_with_sound11(void)
{
    int j;
    int i;
    u32 *vid;
    float volume;
    float scale;
    const struct SoundEffectDesc *desc;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 0x5B8; j++)
        {
            vid = &g_something_with_sound5[i][j];
            volume = lbl_1_bss_21B90[i][j];
            if (g_something_with_sound5[i][j] != -1U)
            {
                *vid = sndFXCheck(*vid);
                scale = g_smth_called_by_event_tick_sound_1(i, j);
                desc = &lbl_1_rodata_148C[j];
                if (desc->flags & 0x4000)
                    g_call_synthFXSetCtrl_with_irq_disabled(*vid, 7, volume * scale);
                else
                    g_call_synthFXSetCtrl_with_irq_disabled(*vid, 7, lbl_1_bss_16030 * (volume * scale));
            }
        }
    }
}
