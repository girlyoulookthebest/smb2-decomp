/* smgr: streams DSP-ADPCM audio from disc into MusyX streams. */
#include "types.h"
#include "libc.h"
#include "file.h"

void *OSAllocFromHeap(void *heap, u32 size);
extern void *currentHeap;
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
void OSReport(const char *fmt, ...);
s32 DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s32 offset, void (*callback)(s32, void *), s32 prio);
u32 sndStreamAllocEx(u8 prio, void *buffer, u32 samples, u32 frq, u8 vol, u8 pan, u8 span, u8 auxa, u8 auxb,
                     u8 studio, u32 flags, u32 (*update)(void *, u32, void *, u32, uptr), uptr user, void *adpcmInfo);

extern s32 lbl_80198B84;  /* DVD drive status */

/* Parameters for starting a stream (filled in by sound.c from the DSP header). */
struct StreamParams
{
    /*0x00*/ char *fileName;
    /*0x04*/ u16 sampleRate;
    /*0x08*/ u32 dataOffset;
    /*0x0C*/ u32 dataSize;
    /*0x10*/ u16 coefs[16];
    /*0x30*/ BOOL loopFlag;
    /*0x34*/ u32 loopStart;
    /*0x38*/ float frameRate;
};

struct SmgrStream
{
    /*0x00*/ u8 state;  /* 0: free, 1: allocated, 2: set up */
    /*0x04*/ u32 id;
    /*0x08*/ struct SmgrStream *right;  /* partner stream, for stereo */
    /*0x0C*/ struct SmgrStream *left;
    /*0x10*/ struct StreamParams *params;
    /*0x14*/ u8 volume;
    /*0x15*/ u8 pan;
    /*0x18*/ struct File file;
    /*0x68*/ u32 sndId;
    /*0x6C*/ u32 readPos;  /* file offset (after the header) of the next read */
    /*0x70*/ u32 dataSize;
    /*0x74*/ u8 *buffer;
    /*0x78*/ u32 bufferSize;
    /*0x7C*/ u32 samples;
    /*0x80*/ u8 bufferHalf;  /* which half of the buffer gets the next read */
    /*0x84*/ BOOL unk84;
    /*0x88*/ BOOL stopped;
    /*0x8C*/ BOOL reading;
    /*0x90*/ BOOL releasePending;
    /*0x94*/ u8 loopCount;
    /*0x98*/ u32 lastLen;
    /*0x9C*/ u8 span;
    /*0xA0*/ u32 unkA0;
    /*0xA4*/ u32 unkA4;
    /*0xA8*/ u32 unkA8;
    /*0xAC*/ s8 unkAC;
    /*0xAD*/ s8 unkAD;
    /*0xB0*/ BOOL dvdMuted;  /* muted while the drive is busy */
    /*0xB4*/ BOOL unkB4;
    /*0xB8*/ BOOL paused;
}; /* size = 0xBC */

struct SmgrState
{
    struct SmgrStream streams[10];
    u32 nextId;
    BOOL irqLevel;
    u32 irqNest;
} lbl_1_bss_25C40;

void sndStreamDeactivate(u32 stid);
void sndStreamARAMUpdate(u32 stid, u32 off1, u32 len1, u32 off2, u32 len2);
void fn_8003F718(u32 stid, u8 vol, u8 pan, u8 span, u8 auxa, u8 auxb);
void fn_1_38A8C(struct SmgrStream *stream);
#define OS_SOUND_MODE_MONO 0u
u32 OSGetSoundMode(void);
BOOL fn_1_393B0(struct SmgrStream *stream);
void sndStreamFrq(u32 stid, u32 frq);
void sndStreamADPCMParameter(u32 stid, void *adpcmInfo);
u32 sndStreamActivate(u32 stid);
u32 fn_1_38E98(void *buffer1, u32 len1, void *buffer2, u32 len2, uptr user);
void g_some_dvd_read_async_sound_callback(s32 result, void *fileInfo);
s32 DVDGetCommandBlockStatus(void *block);

static inline void smgr_lock(void)
{
    if (lbl_1_bss_25C40.irqNest++ == 0)
        lbl_1_bss_25C40.irqLevel = OSDisableInterrupts();
}

static inline void smgr_unlock(void)
{
    if (lbl_1_bss_25C40.irqNest != 0 && --lbl_1_bss_25C40.irqNest == 0)
        OSRestoreInterrupts(lbl_1_bss_25C40.irqLevel);
}

void fn_1_375A8(void)
{
    u32 i;

    for (i = 0; i < 10; i++)
    {
        lbl_1_bss_25C40.streams[i].state = 0;
        lbl_1_bss_25C40.streams[i].buffer = OSAllocFromHeap(currentHeap, 0x12780);
        lbl_1_bss_25C40.streams[i].bufferSize = 0x12780;
        lbl_1_bss_25C40.streams[i].samples = 0x20520;
        lbl_1_bss_25C40.streams[i].sndId = sndStreamAllocEx(0xFF, lbl_1_bss_25C40.streams[i].buffer,
                                                            lbl_1_bss_25C40.streams[i].samples, 44100, 0, 0x40,
                                                            0, 0, 0, 0, 0x30001, fn_1_38E98,
                                                            (uptr)&lbl_1_bss_25C40.streams[i], NULL);
    }
    lbl_1_bss_25C40.nextId = 0;
    lbl_1_bss_25C40.irqNest = 0;
}

u32 g_something_with_sound4(struct SmgrStream *stream, struct StreamParams *params, u8 volume, u8 pan, u8 span,
                            s8 unkAD)
{
    u32 len;
    u32 half;

    stream->params = params;
    stream->volume = volume;
    stream->pan = pan;
    stream->unk84 = 1;
    stream->stopped = 0;
    stream->releasePending = 0;
    stream->reading = 1;
    stream->bufferHalf = 1;
    if (stream->params->sampleRate > 44100)
        OSReport("!! STREAM ERROR !!\n %s frq is %d > MAX_FRQ %d", stream->params->fileName,
                 stream->params->sampleRate, 44100);
    stream->bufferSize = ((stream->params->sampleRate * 24) / 14 + 63) & ~63;
    stream->samples = (stream->bufferSize >> 3) * 14;
    if (stream->bufferSize > 0x12780)
        stream->bufferSize = 0x12780;
    half = stream->bufferSize >> 1;
    stream->readPos = half;
    stream->dataSize = stream->params->dataSize;
    if (stream->dataSize < half)
    {
        len = stream->dataSize;
        memset(stream->buffer + stream->dataSize, 0, half - stream->dataSize);
    }
    else
        len = half;
    stream->state = 2;
    stream->loopCount = 0;
    stream->lastLen = 0;
    stream->span = span;
    stream->unkA0 = 0;
    stream->unkA4 = 0;
    stream->unkA8 = 0;
    stream->unkAC = 0;
    stream->unkAD = unkAD;
    stream->dvdMuted = FALSE;
    stream->unkB4 = 0;
    return len;
}

/* Find a free stream and give it a new unique ID. */
static inline u32 smgr_alloc_stream(struct SmgrStream **out)
{
    u32 i;
    u32 j;
    u32 id;

    for (i = 0; i < 10; i++)
    {
        if (lbl_1_bss_25C40.streams[i].state == 0)
        {
            do
            {
                id = lbl_1_bss_25C40.nextId++;
                if (id == -2U)
                    id = lbl_1_bss_25C40.nextId++;
                if (id == -1U)
                    id = lbl_1_bss_25C40.nextId++;
                for (j = 0; j < 10; j++)
                {
                    if (lbl_1_bss_25C40.streams[j].state != 0 && id == lbl_1_bss_25C40.streams[j].id)
                        break;
                }
            } while (j != 10);
            lbl_1_bss_25C40.streams[i].state = 1;
            lbl_1_bss_25C40.streams[i].id = id;
            lbl_1_bss_25C40.streams[i].right = NULL;
            lbl_1_bss_25C40.streams[i].left = NULL;
            *out = &lbl_1_bss_25C40.streams[i];
            return id;
        }
    }
    return -1;
}

static inline void smgr_alloc_stream_id(u32 *id, struct SmgrStream **out)
{
    u32 j;
    u32 i;

    for (i = 0; i < 10; i++)
    {
        if (lbl_1_bss_25C40.streams[i].state == 0)
        {
            do
            {
                {
                    struct SmgrState *state = &lbl_1_bss_25C40;

                    *id = state->nextId;
                    state->nextId = *id + 1;
                }
                if (*id == -2U)
                {
                    struct SmgrState *state = &lbl_1_bss_25C40;

                    *id = state->nextId;
                    state->nextId = *id + 1;
                }
                if (*id == -1U)
                {
                    struct SmgrState *state = &lbl_1_bss_25C40;

                    *id = state->nextId;
                    state->nextId = *id + 1;
                }
                for (j = 0; j < 10; j++)
                {
                    if (lbl_1_bss_25C40.streams[j].state != 0 && *id == lbl_1_bss_25C40.streams[j].id)
                        break;
                }
            } while (j != 10);
            lbl_1_bss_25C40.streams[i].state = 1;
            lbl_1_bss_25C40.streams[i].id = *id;
            lbl_1_bss_25C40.streams[i].right = NULL;
            lbl_1_bss_25C40.streams[i].left = NULL;
            *out = &lbl_1_bss_25C40.streams[i];
            return;
        }
    }
    *id = -1;
}

static inline BOOL dvd_busy(void)
{
    BOOL busy = FALSE;

    if (lbl_80198B84 != 0 && lbl_80198B84 != 11)
        busy = TRUE;
    return busy;
}

/* smgrAllocate: with channel -1, reserve a stream; otherwise set up the stream on
 * *channel and start reading its file. */
u32 fn_1_378D4(struct StreamParams *params, u8 volume, u8 pan, u8 span, s8 *channel, s8 index)
{
    u32 id;
    struct SmgrStream *stream;

    smgr_lock();
    if (index == -1)
    {
        id = smgr_alloc_stream(&stream);
        if (id != -1U)
        {
            smgr_unlock();
            *channel = stream->sndId;
            stream->loopCount = 0;
            stream->unkA4 = 0;
            stream->paused = 0;
            return id;
        }
        smgr_unlock();
        return id;
    }
    stream = &lbl_1_bss_25C40.streams[*channel];
    id = g_something_with_sound4(stream, params, volume, pan, span, 100);
    smgr_unlock();
    if (!file_open(stream->params->fileName, &stream->file))
    {
        OSReport("Can't find %s at smgrAllocate()\n", stream->params->fileName);
        return 0;
    }
    if (!dvd_busy())
        DVDReadAsyncPrio(&stream->file.fileInfo, stream->buffer, id, stream->params->dataOffset,
                         g_some_dvd_read_async_sound_callback, 2);
    return 0;
}

/* smgrAllocateStereo: like fn_1_378D4, for a left/right pair of streams.
 * NONMATCHING: left/id and channelL have their registers (r27/r29) swapped. */
u32 g_smgr_allocate_stereo(struct StreamParams *left, struct StreamParams *right, u8 volume, s8 unkAD,
                           s8 *channelL, s8 *channelR, s8 index)
{
    struct SmgrStream *streamL;
    struct SmgrStream *streamR;
    u32 id;
    u32 sizeR;

    smgr_lock();
    if (index == -1)
    {
        u32 idR;

        smgr_alloc_stream_id(&id, &streamL);
        if (id != -1U)
        {
            smgr_alloc_stream_id(&idR, &streamR);
            if (idR != -1U)
            {
                smgr_unlock();
                *channelL = streamL->sndId;
                *channelR = streamR->sndId;
                streamL->loopCount = 0;
                streamL->unkA4 = 0;
                streamL->paused = 0;
                streamR->paused = 0;
                return id;
            }
            streamL->state = 0;
            id = -1;
        }
        smgr_unlock();
        return id;
    }
    streamL = &lbl_1_bss_25C40.streams[*channelL];
    streamR = &lbl_1_bss_25C40.streams[*channelR];
    streamL->right = streamR;
    streamR->left = streamL;
    id = g_something_with_sound4(streamL, left, volume, 0, 0, unkAD);
    sizeR = g_something_with_sound4(streamR, right, volume, 0x7F, 0, unkAD);
    smgr_unlock();
    if (!file_open(streamL->params->fileName, &streamL->file))
    {
        OSReport("Can't find %s at smgrAllocateStereo() L\n", streamL->params->fileName);
        return 0;
    }
    if (!file_open(streamR->params->fileName, &streamR->file))
    {
        OSReport("Can't find %s at smgrAllocateStereo() R\n", streamR->params->fileName);
        return 0;
    }
    if (!dvd_busy())
    {
        DVDReadAsyncPrio(&streamL->file.fileInfo, streamL->buffer, id, streamL->params->dataOffset,
                         g_some_dvd_read_async_sound_callback, 2);
        DVDReadAsyncPrio(&streamR->file.fileInfo, streamR->buffer, sizeR, streamR->params->dataOffset,
                         g_some_dvd_read_async_sound_callback, 2);
    }
    return 0;
}

static inline u32 smgr_find(u32 id)
{
    u32 i;

    for (i = 0; i < 10; i++)
    {
        if (lbl_1_bss_25C40.streams[i].state != 0 && id == lbl_1_bss_25C40.streams[i].id)
            return i;
    }
    return -1;
}

static inline void smgr_stop_stream(struct SmgrStream *stream)
{
    if (stream->state != 0 && stream->stopped == 0)
    {
        fn_8003F718(stream->sndId, 0, stream->pan, stream->span, 0, 0);
        stream->stopped = 1;
        if (stream->paused != 0)
            fn_1_38A8C(stream);
    }
}

/* smgrStop: silence a stream (and its stereo partner). */
void fn_1_38220(u32 id)
{
    u32 index;

    smgr_lock();
    index = smgr_find(id);
    if (index != -1U)
    {
        smgr_stop_stream(&lbl_1_bss_25C40.streams[index]);
        if (lbl_1_bss_25C40.streams[index].right != NULL)
            smgr_stop_stream(lbl_1_bss_25C40.streams[index].right);
        if (lbl_1_bss_25C40.streams[index].left != NULL)
            smgr_stop_stream(lbl_1_bss_25C40.streams[index].left);
    }
    smgr_unlock();
}

/* Returns whether a stream ID is still in use. */
BOOL fn_1_384EC(u32 id)
{
    BOOL active = FALSE;
    u32 index;

    smgr_lock();
    index = smgr_find(id);
    if (index != -1U)
        active = lbl_1_bss_25C40.streams[index].state != 0;
    smgr_unlock();
    return active;
}

/* Set a stream's volume (percent of its base volume). */
void fn_1_38618(int channel, s8 volume)
{
    u8 pan;
    u8 span;
    struct SmgrStream *stream = &lbl_1_bss_25C40.streams[channel];

    volume = (volume < 0) ? 0 : (volume > 100) ? 100 : volume;
    stream->unkAD = volume;
    pan = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0x40 : stream->pan;
    span = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0 : stream->span;
    fn_8003F718(stream->sndId, stream->volume * (0.01f * volume), pan, span, 0, 0);
}

void fn_1_3874C(int channel, s8 volume)
{
    u8 pan;
    u8 span;
    struct SmgrStream *stream = &lbl_1_bss_25C40.streams[channel];

    stream->volume = volume;
    pan = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0x40 : stream->pan;
    span = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0 : stream->span;
    fn_8003F718(stream->sndId, stream->volume, pan, span, 0, 0);
}

void fn_1_387E0(int channel, s8 pan, s8 span)
{
    struct SmgrStream *stream = &lbl_1_bss_25C40.streams[channel];

    stream->pan = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0x40 : pan;
    stream->span = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0 : span;
    fn_8003F718(stream->sndId, stream->volume, stream->pan, stream->span, 0, 0);
}

void fn_1_38888(int channel, s8 value)
{
    struct SmgrStream *stream = &lbl_1_bss_25C40.streams[channel];

    stream->unkAC = value;
}

/* Reapply a stream's volume and pan (after the sound mode changed). */
void fn_1_388A0(int channel)
{
    u8 pan;
    u8 span;
    struct SmgrStream *stream = &lbl_1_bss_25C40.streams[channel];

    pan = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0x40 : stream->pan;
    span = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0 : stream->span;
    fn_8003F718(stream->sndId, stream->volume, pan, span, 0, 0);
}

u32 fn_1_3892C(int channel)
{
    return lbl_1_bss_25C40.streams[channel].unkA0;
}

u32 fn_1_38944(int channel)
{
    return lbl_1_bss_25C40.streams[channel].unkA4;
}

u32 fn_1_3895C(int channel)
{
    return lbl_1_bss_25C40.streams[channel].unkA8;
}

BOOL fn_1_38974(int channel)
{
    return lbl_1_bss_25C40.streams[channel].state >= 4;
}

/* smgrStartWait: pause or resume a stream. */
void fn_1_389AC(int channel, BOOL pause)
{
    if (pause == FALSE && lbl_1_bss_25C40.streams[channel].paused)
    {
        if (lbl_1_bss_25C40.streams[channel].state == 2)
            fn_1_393B0(&lbl_1_bss_25C40.streams[channel]);
        else
        {
            OSReport("STREAM WARNING stid:%d at smgrStartWait\n", lbl_1_bss_25C40.streams[channel].sndId);
            return;
        }
    }
    lbl_1_bss_25C40.streams[channel].paused = pause;
}

BOOL fn_1_38A74(int channel)
{
#ifndef __MWERKS__
    /* g_change_music_volume passes the -1 of a track that is not playing; on
     * GC streams[-1].paused is the zero padding word after world_sound.c's
     * lbl_1_bss_25C38, just before this bss */
    if (channel < 0)
        return FALSE;
#endif
    return lbl_1_bss_25C40.streams[channel].paused;
}

/* Stop a stream's playback and free it (or flag it, if a read is in flight). */
void fn_1_38A8C(struct SmgrStream *stream)
{
    sndStreamDeactivate(stream->sndId);
    if (!stream->reading)
    {
        file_close(&stream->file);
        stream->state = 0;
        stream->dataSize = 0;
        stream->readPos = 0;
        stream->loopCount = 0;
        stream->unkA8 = 0;
    }
    else
        stream->releasePending = TRUE;
}

/* Read the next chunk of a stream into the free half of its buffer. */
void fn_1_38B04(struct SmgrStream *stream)
{
    u32 len;
    u8 *dest;
    BOOL restart = FALSE;
    BOOL ended = FALSE;
    u32 half;

    if (dvd_busy())
        return;
    half = stream->bufferSize >> 1;
    len = half;
    dest = stream->bufferHalf ? stream->buffer + half : stream->buffer;
    if (stream->readPos + len > stream->dataSize)
    {
        if (stream->dataSize > stream->readPos)
        {
            len = stream->lastLen = stream->dataSize - stream->readPos;
            if (!stream->params->loopFlag)
                memset(dest + len, 0, half - len);
            ended = TRUE;
        }
        else
        {
            if (!stream->params->loopFlag)
            {
                memset(dest, 0, half);
                sndStreamARAMUpdate(stream->sndId, stream->bufferHalf ? stream->samples >> 1 : 0,
                                    stream->samples >> 1, 0, 0);
                stream->unkB4 = TRUE;
                len = 0;
            }
            restart = TRUE;
        }
    }
    else if (stream->dataSize == stream->readPos + len)
        stream->lastLen = len;

    if (!restart)
    {
        if (ended && stream->params->loopFlag)
            stream->state = 5;
        if (!dvd_busy())
        {
            DVDReadAsyncPrio(&stream->file.fileInfo, dest, len, stream->params->dataOffset + stream->readPos,
                             g_some_dvd_read_async_sound_callback, 2);
            stream->reading = TRUE;
            stream->readPos += len;
            if (stream->state != 5)
                stream->unkB4 = FALSE;
        }
    }
    else if (stream->params->loopFlag)
    {
        stream->readPos = stream->params->loopStart + (len - stream->lastLen);
        if (!dvd_busy())
        {
            DVDReadAsyncPrio(&stream->file.fileInfo, dest, len, stream->params->dataOffset + stream->readPos,
                             g_some_dvd_read_async_sound_callback, 2);
            stream->reading = TRUE;
            stream->readPos += len;
            stream->loopCount++;
            stream->unkB4 = FALSE;
        }
    }
    else if (stream->unk84)
        stream->unk84 = FALSE;
    else
        fn_1_38A8C(stream);
    if (!dvd_busy())
        stream->bufferHalf ^= 1;
}

/* MusyX stream update callback: keeps the buffer fed and tracks playback time. */
u32 fn_1_38E98(void *buffer1, u32 len1, void *buffer2, u32 len2, uptr user)
{
    struct SmgrStream *stream = (struct SmgrStream *)user;
    s8 savedVolume;

    if (dvd_busy())
    {
        if (!stream->dvdMuted)
        {
            savedVolume = stream->unkAD;
            fn_1_38618(stream->sndId, 0);
            stream->unkAD = savedVolume;
            stream->dvdMuted = dvd_busy();
        }
    }
    else if (stream->dvdMuted)
    {
        fn_1_38618(stream->sndId, stream->unkAD);
        stream->dvdMuted = dvd_busy();
    }
    if (dvd_busy())
    {
        if (stream->dataSize == 0x1ABA0)
            fn_1_38A8C(stream);
        return 0;
    }
    stream->unkA0++;
    if (1.2f * (float)(stream->unkA0 + stream->unkAC) >= stream->params->frameRate * (float)(stream->unkA4 + 1))
    {
        stream->unkA4++;
        stream->unkA8 = 1;
    }
    else
        stream->unkA8 = 0;
    if (stream->stopped)
    {
        fn_1_38A8C(stream);
        return 0;
    }
    if (!stream->unkB4)
        return 0;
    if (stream->right != NULL && !stream->right->unkB4)
        return 0;
    if (len1 + len2 < stream->samples >> 1)
        return 0;
    fn_1_38B04(stream);
    return stream->samples >> 1;
}

/* Start a set-up stream playing. */
BOOL fn_1_393B0(struct SmgrStream *stream)
{
    s8 pan;
    s8 span;

    pan = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0x40 : stream->pan;
    span = (OSGetSoundMode() == OS_SOUND_MODE_MONO) ? 0 : stream->span;
    fn_8003F718(stream->sndId, stream->volume * (0.01f * stream->unkAD), pan, span, 0, 0);
    sndStreamFrq(stream->sndId, stream->params->sampleRate);
    sndStreamADPCMParameter(stream->sndId, stream->params->coefs);
    sndStreamARAMUpdate(stream->sndId, 0, stream->samples >> 1, 0, 0);
    stream->unkB4 = TRUE;
    if (sndStreamActivate(stream->sndId))
    {
        stream->state = 4;
        fn_1_38B04(stream);
        return TRUE;
    }
    file_close(&stream->file);
    stream->state = 0;
    stream->dataSize = 0;
    stream->readPos = 0;
    stream->loopCount = 0;
    stream->unkA8 = 0;
    return FALSE;
}

/* DVD read completion for stream data: starts set-up streams (both channels of a
 * stereo pair together) and hands finished reads to the stream. */
void g_some_dvd_read_async_sound_callback(s32 result, void *fileInfo)
{
    u32 i;
    struct SmgrStream *stream;
    u32 half;
    u8 *dest;

    for (i = 0; i < 10; i++)
    {
        if ((void *)&lbl_1_bss_25C40.streams[i].file.fileInfo == fileInfo)
            break;
    }
    stream = &lbl_1_bss_25C40.streams[i];
    stream->reading = FALSE;
    if (stream->releasePending)
    {
        stream->releasePending = FALSE;
        fn_1_38A8C(stream);
        return;
    }
    if (DVDGetCommandBlockStatus(fileInfo) == 0)
    {
        switch (stream->state)
        {
        case 2:
            if (stream->paused)
                return;
            if (stream->right != NULL)
            {
                if (stream->right->state != 3)
                {
                    stream->state = 3;
                    return;
                }
                if (!fn_1_393B0(stream->right))
                    stream->right = NULL;
            }
            if (stream->left != NULL)
            {
                if (stream->left->state != 3)
                {
                    stream->state = 3;
                    return;
                }
                if (!fn_1_393B0(stream->left))
                    stream->left = NULL;
            }
            if (!fn_1_393B0(stream))
            {
                if (stream->right != NULL)
                    stream->right->left = NULL;
                if (stream->left != NULL)
                    stream->left->right = NULL;
            }
            break;
        case 4:
            sndStreamARAMUpdate(stream->sndId, stream->bufferHalf ? 0 : stream->samples >> 1,
                                stream->samples >> 1, 0, 0);
            stream->unkB4 = TRUE;
            break;
        case 5:
            half = stream->bufferSize >> 1;
            dest = (u8 *)(stream->lastLen + (uptr)(stream->bufferHalf ? stream->buffer : stream->buffer + half));
            if (!stream->unkB4)
                break;
            if (stream->right != NULL && !stream->right->unkB4)
                break;
            if (!dvd_busy())
            {
                DVDReadAsyncPrio(&stream->file.fileInfo, dest, half - stream->lastLen,
                                 stream->params->dataOffset + stream->params->loopStart,
                                 g_some_dvd_read_async_sound_callback, 2);
                stream->reading = TRUE;
                stream->state = 4;
                stream->unkB4 = FALSE;
            }
            break;
        }
    }
    else
    {
        if (stream->state >= 4)
            sndStreamDeactivate(stream->sndId);
        fn_1_38A8C(stream);
        if (stream->right != NULL)
            stream->right->left = NULL;
        if (stream->left != NULL)
            stream->left->right = NULL;
    }
}
