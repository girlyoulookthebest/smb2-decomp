/* Monkey Fight 2: main mode/state logic (fight_3_11300.inc) and per-frame player logic
 * (fight_3_14914.inc). */
#include "fight.h"
#include "nl2ngc.h"

#ifdef __MWERKS__
#define FIGHT_OFFSETOF(type, member) ((u32)&((type *)0)->member)
#else
#include <stddef.h>
#define FIGHT_OFFSETOF(type, member) offsetof(type, member)
#endif

/* ---- fight_3 data (owner: w5) ---- */

/* state functions (defined in the fragments below) */
void fn_12_11FF8(void);
void fn_12_120B0(void);
void fn_12_120C4(void);
void fn_12_1212C(void);
void fn_12_12140(void);
void fn_12_12604(void);
void fn_12_127E8(void);
void fn_12_1292C(void);
void fn_12_12AB8(void);
void fn_12_12CBC(void);
void fn_12_13FEC(void);
void fn_12_14448(void);
void fn_12_14878(void);
void fn_12_14910(void);
void fn_12_14914(void);
void fn_12_14E28(void);
void fn_12_13708(void);
void fn_12_13990(void);
void fn_12_1315C(void);
void fn_12_13234(void);
void fn_12_13AA4(void);
void fn_12_13C84(void);
void fn_12_14F48(void);

/* .data 0x608 */
void (*lbl_12_data_608[])(void) =
{
    fn_12_11FF8,  /* 0x00 */
    fn_12_120B0,  /* 0x01 */
    fn_12_120C4,  /* 0x02 */
    fn_12_1212C,  /* 0x03 */
    fn_12_12140,  /* 0x04 round init */
    fn_12_12604,  /* 0x05 */
    fn_12_127E8,  /* 0x06 */
    fn_12_1292C,  /* 0x07 */
    fn_12_12AB8,  /* 0x08 */
    fn_12_12CBC,  /* 0x09 */
    fn_12_13FEC,  /* 0x0A */
    fn_12_14448,  /* 0x0B */
    fn_12_14878,  /* 0x0C */
    fn_12_14910,  /* 0x0D */
    fn_12_14914,  /* 0x0E */
    fn_12_14E28,  /* 0x0F */
    fn_12_13708,  /* 0x10 */
    fn_12_13990,  /* 0x11 */
    fn_12_1315C,  /* 0x12 */
    fn_12_13234,  /* 0x13 */
    fn_12_13AA4,  /* 0x14 */
    fn_12_13C84,  /* 0x15 */
    fn_12_14F48,  /* 0x16 */
    NULL,
};

/* .data 0x668: stage per round slot */
struct FightRoundStage lbl_12_data_668[3] =
{
    { 1, 0x8F },
    { 4, 0x90 },
    { 5, 0x91 },
};

/* .data 0x674: round announcement sound per round */
s32 lbl_12_data_674[20] =
{
    0x186, 0x187, 0x188, 0x189, 0x18A, 0x18B, 0x18C, 0x18D, 0x18E, 0x18F,
    0x190, 0x191, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x198, 0x199,
};

/* .data 0x6C4: init_common_gma model per player */
u16 lbl_12_data_6C4[4] = { 1, 3, 5, 7 };

/* .data 0x6CC: per player colors */
GXColor lbl_12_data_6CC[4] =
{
    { 0x60, 0x80, 0x80, 0xFF },
    { 0x80, 0x80, 0x60, 0xFF },
    { 0x60, 0x60, 0x80, 0xFF },
    { 0x80, 0x60, 0x80, 0xFF },
};

/* .bss 0x1ACA0-0x1EDF0 */
struct FightWork lbl_12_bss_1ACA0;
u8 lbl_12_bss_1C404;
s8 lbl_12_bss_1C408[4];
u8 lbl_12_bss_1C40C[4];
s8 lbl_12_bss_1C410[3];
struct GMAModel *lbl_12_bss_1C414;
struct GMAModel *lbl_12_bss_1C418[4];
struct FightMarker lbl_12_bss_1C428[4];
struct Fight1C518 lbl_12_bss_1C518[17];
u8 lbl_12_bss_1C5E8[0x2804] __attribute__((aligned(8)));  /* 8-aligned in the original (0x1C5E8) */

#include "fight/fight_3_11300.inc"
#include "fight/fight_3_14914.inc"
