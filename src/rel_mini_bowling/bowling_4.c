#include "types.h"

int call_SoundReqID_arg_1_copy(u32 soundId);
void SoundVol(u16 soundId, u8 volume);
void SoundPan(u16 soundId, s8 pan, s8 span);
void SoundPitch(u16 soundId, u16 pitch);

void fn_5_119B0(u32 soundId, float *pan, int vol, u16 pitch)
{
    int handle;
    int panValue = 0.9 * *pan;

    handle = call_SoundReqID_arg_1_copy(soundId);
    SoundVol(handle, vol + 100);
    SoundPan(handle, panValue, -0x3F);
    SoundPitch(handle, pitch);
}
