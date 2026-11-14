#ifndef SEL_H
#define SEL_H

#include "types.h"

/* Menu navigation stack for the main menu ("sel"). */
struct SelMenuInfo
{
    /*0x00*/ s8 menuStackPtr;
    /*0x01*/ u8 menuStack[10];
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0E*/ s16 unkE;
}; /* size = 0x10 */

extern struct SelMenuInfo sel_menu_info;

#endif
