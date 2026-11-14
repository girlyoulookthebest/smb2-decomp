#ifndef SHADOW_H
#define SHADOW_H

#include "types.h"
#include "gx.h"
#include "mathutil.h"

struct GMAModel;

/* Blob shadow request (SMB1's Struct8009492C), submitted by ball_draw and item code. */
struct ShadowReq
{
    /*0x00*/ Vec pos;
    /*0x0C*/ S16Vec rot;
    /*0x14*/ Vec scale;
    /*0x20*/ float unk20;
    /*0x24*/ float unk24;
    /*0x28*/ struct GMAModel *model;
    /*0x2C*/ GXColor color;
    /*0x30*/ float unk30;
    /*0x34*/ float unk34;
};  /* size = 0x38 */

void vec_to_euler(Vec *v, S16Vec *rot);
int submit_shadow_req(struct ShadowReq *req);

#endif
