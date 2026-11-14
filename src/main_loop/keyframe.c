/* Stagedef animation curves (SMB1 interpolate_keyframes.c). */
#include "types.h"
#include "stage.h"

float evaluate_stagedef_keyframe(float time, int count, struct StageKeyframe *frames)
{
    struct StageKeyframe *curr;
    struct StageKeyframe *next;
    int i;
    float dt;  // time between the current key and the next one
    float t;  // interpolation factor
    float t2;  // t * t
    float t3;  // t * t * t

    if (count < 2 || time <= frames[0].time)
        return frames[0].value;
    if (time >= frames[count - 1].time)
        return frames[count - 1].value;

    next = &frames[1];
    for (i = 1; i < count - 1; i++, next++)
    {
        if (next->time > time)
            break;
    }
    curr = next - 1;

    switch (curr->easeType)
    {
    case 0:  // step
        return curr->value;
    case 1:  // linear
        dt = next->time - curr->time;
        t = (time - curr->time) / dt;
        return curr->value * (1.0 - t) + next->value * t;
    default:  // Hermite
        dt = next->time - curr->time;
        t = (time - curr->time) / dt;
        t2 = t * t;
        t3 = t2 * t;
        return (float)(curr->value * (1.0 + (2.0 * t3 - 3.0 * t2)) + next->value * (-2.0 * t3 + 3.0 * t2))
             + dt * ((curr->tangentOut * (t + (t3 - 2.0 * t2))) + next->tangentIn * (t3 - t2));
    }
}
