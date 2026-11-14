#ifndef SOUND_H
#define SOUND_H

#include "types.h"

/* MusyX sound group files (snd/mkb/<baseName>.pool/.proj/.samp/.sdir). */
struct SoundGroupDesc
{
    /*0x00*/ char *name;
    /*0x04*/ s32 firstSoundId;
    /*0x08*/ s32 groupId;
    /*0x0C*/ s32 slot;  /* index into the loaded data arrays */
    /*0x10*/ char *baseName;
}; /* size = 0x14 */

/* One sound effect (indexed by sound ID). */
struct SoundEffectDesc
{
    /*0x00*/ u16 fxId;
    /*0x02*/ u16 unk2;
    /*0x04*/ char *name;
    /*0x08*/ u16 flags;
    /*0x0A*/ s16 repeatDelay;
    /*0x0C*/ s16 cueId;
    /*0x0E*/ u16 unkE;
}; /* size = 0x10 */

extern const struct SoundGroupDesc lbl_1_rodata_1284[];
extern const struct SoundEffectDesc lbl_1_rodata_148C[];
extern s32 lbl_1_bss_217EC;  /* current sound group set */
extern s32 lbl_1_bss_217F0;
extern s8 lbl_1_bss_217FD;
extern s8 lbl_1_bss_24980[];  /* per-sound volume adjustment */
extern s8 lbl_1_bss_25918[];  /* per-track volume adjustment */
extern BOOL lbl_1_bss_259AC;  /* aux effects active */

int SoundReqDirect(u32 soundId);
void SoundOff(u16 soundId);

#endif
