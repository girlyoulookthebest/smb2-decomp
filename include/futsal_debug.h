#ifndef FUTSAL_DEBUG_H
#define FUTSAL_DEBUG_H

/* Futsal debug name tables. Defined `static` in a shared header: identical copies
 * are emitted into every file that includes it (seen in main_loop's camera.c and in
 * rel_mini_futsal). Table names are placeholders; contents are verbatim. */

static const char futsalFieldStateNames[][14] = {
    "FIELD_DRIBBLE", "FIELD_LOOSE  ", "FIELD_PASS   ", "FIELD_PASSGET",
    "FIELD_SHOOT  ", "FIELD_HALFTIM", "FIELD_PK     ",
};

static const char futsalGameStateNames[][12] = {
    "VERSUSDISP ", "KICKOFF    ", "GAME       ", "REPLAY     ",
    "PERFORM    ", "RESTART    ", "HALFTIME   ", "PK_COIN    ",
    "PK_RESTART ", "PK         ", "GAMEOVER   ", "VER_RESTART",
    "COMMEND    ", "BALLTEST   ", "DEBUGREPLAY",
};

static const char futsalPlayerActionNames[][8] = {
    "DRIBBLE", "PASS   ", "SHOOT  ", "PASSGET", "BASE   ", "PATH   ",
    "DEFENSE", "TACKLE ", "COVER  ", "KEEPER ", "PK_KEP ", "PK_KICK",
};

static const char futsalRobotMoveNames[][16] = {
    "ROB_MOVE_STOP  ", "ROB_MOVE_DIRCHG", "ROB_MOVE_TOSTOP", "ROB_MOVE_SPD   ", "ROB_MOVE_ACC   ",
};

/* Split into two 8-byte tables is a guess; the bytes are what matter. */
static const u8 futsalUnkTable1[8] = { 2, 0, 1, 2, 3, 4, 7, 8 };
static const u8 futsalUnkTable2[8] = { 0, 1, 4, 5, 8, 9, 12, 0 };
 
#include "futsal_data.h"

#endif
