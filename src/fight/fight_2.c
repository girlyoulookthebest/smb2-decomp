#include "fight.h"
#include "nl2ngc.h"  /* struct OrdTblNode (ball trail draw node) */

/* 0x8ED0-0x9080: effect func install/restore and .data 0x390-0x5F0 */
#include "fight/fight_2_8ED0.inc"

/* ---- fight_2 data (owner: w3) ---- */

/* minigameGma model index of each digit 0-9 (score number effect, fn_12_C66C) */
u16 lbl_12_data_5F0[10] = { 0x8B, 0x70, 0x83, 0x82, 0x62, 0x61, 0x7E, 0x7C, 0x5D, 0x6E };

#include "fight/fight_2_9080.inc"
#include "fight/fight_2_CA84.inc"
