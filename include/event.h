#ifndef EVENT_H
#define EVENT_H

#include "types.h"

struct Event
{
    /*0x00*/ s8 state;
    /*0x04*/ char *name;
    /*0x08*/ void (*start)(void);
    /*0x0C*/ void (*main)(void);
    /*0x10*/ void (*finish)(void);
    /*0x14*/ u32 time;
}; /* size = 0x18 */

enum
{
    EVENT_STAGE,
    EVENT_WORLD,
    EVENT_BALL,
    EVENT_APE,
    EVENT_STOBJ,
    EVENT_ITEM,
    EVENT_RECPLAY,
    EVENT_OBJ_COLLISION,
    EVENT_NAME_ENTRY,
    EVENT_INFO,
    EVENT_COURSE,
    EVENT_VIBRATION,
    EVENT_COMMEND,
    EVENT_VIEW,
    EVENT_EFFECT,
    EVENT_MINIMAP,
    EVENT_CAMERA,
    EVENT_SPRITE,
    EVENT_MOUSE,
    EVENT_SOUND,
    EVENT_BACKGROUND,
    EVENT_REND_EFC,
    EVENT_ADX,
};

enum
{
    EV_STATE_INACTIVE,
    EV_STATE_INITIALIZING,
    EV_STATE_RUNNING,
    EV_STATE_FINISHING,
    EV_STATE_SUSPENDED,
};

extern struct Event events[];

#endif
