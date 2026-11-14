#pragma peephole off
#include "types.h"

/* Same as sound.c's and hud_ingame.c's curves. Terminated by a frame of 0xFFFF. */
struct Keyframe
{
    s32 frame;
    float value;
    float tangentIn;
    float tangentOut;
};

float fn_1_D2550(float time, struct Keyframe *frames)
{
    float dt;
    float t;
    float t2;
    float t3;

    if (time < (float)frames[0].frame)
        return frames[0].value;
    frames++;
    while (time > (float)frames->frame)
    {
        if (frames->frame == 0xFFFF)
        {
            frames--;
            break;
        }
        frames++;
    }
    frames--;
    dt = (float)(frames[1].frame - frames[0].frame);
    t = (time - (float)frames[0].frame) / dt;
    t2 = t * t;
    t3 = t2 * t;
    time = frames[0].value * (1.0 + (2.0 * t3 - 3.0 * t2)) + frames[1].value * (-2.0 * t3 + 3.0 * t2);
    return time + dt * ((frames[0].tangentOut * (t + (t3 - 2.0 * t2))) + frames[1].tangentIn * (t3 - t2));
}
