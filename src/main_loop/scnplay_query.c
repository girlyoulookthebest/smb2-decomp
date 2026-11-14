#include "types.h"
#include "mathutil.h"

struct ScenePlayer
{
    /*0x00*/ void *instances;
    /*0x04*/ s32 count;
    /*0x08*/ s32 stage;
    /*0x0C*/ s32 unkC;
    /*0x10*/ u16 sceneId;
    /*0x12*/ u16 frame;
    /*0x14*/ u16 duration;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1C*/ Vec unk1C;
    /*0x28*/ Vec unk28;
    /*0x34*/ u8 lightCount;
    /*0x35*/ u8 done;
};  /* size = 0x38 */

struct DialogState
{
    s32 unk0;
    u8 unk4[0xC];
    u8 unk10[0x84];
    s32 unk94;
    s32 unk98;
};

struct SceneDrawState
{
    s32 paused;
    s32 reset;
};

extern struct ScenePlayer lbl_1_bss_DD4F4;

void fn_1_160B60(void *a, int b);
void fn_1_160D00(void *a);

float fn_1_15F720(void)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    float t = (float)player->frame / (float)player->duration;

    switch (player->unkC)
    {
    case 6:
        switch (player->sceneId)
        {
        case 4:
        case 5:
        case 6:
            t = -1.0f;
            break;
        case 8:
            t *= 5.0f;
            break;
        case 12:
            t = 1.0f;
            break;
        }
        break;
    }
    if (t > 1.0f)
        t = 1.0f;
    return t;
}

int fn_1_15F7E4(void)
{
    int ret = 0;
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;

    switch (player->unkC)
    {
    case 6:
        switch (player->sceneId)
        {
        case 8:
            ret = 1;
            break;
        }
        break;
    case 7:
        switch (player->sceneId)
        {
        case 4:
        case 14:
            ret = 1;
            break;
        }
        break;
    }
    return ret;
}

int fn_1_15F84C(void)
{
    int ret = 0;
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;

    switch (player->unkC)
    {
    case 6:
        switch (player->sceneId)
        {
        case 8:
            ret = 1;
            break;
        }
        break;
    case 7:
        switch (player->sceneId)
        {
        case 3:
        case 4:
            ret = 1;
            break;
        }
        break;
    }
    return ret;
}

int fn_1_15F8B0(void)
{
    int ret = 0;
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;

    switch (player->unkC)
    {
    case 2:
        switch (player->sceneId)
        {
        case 16:
        case 19:
        case 20:
        case 21:
            ret = 1;
            break;
        case 14:
            if (player->frame < 240)
                ret = 1;
            break;
        }
        break;
    case 3:
        switch (player->sceneId)
        {
        case 11:
        case 13:
        case 18:
        case 21:
        case 22:
            ret = 1;
            break;
        }
        break;
    case 4:
        switch (player->sceneId)
        {
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            ret = 1;
            break;
        }
        break;
    }
    return ret;
}

void fn_1_15F984(struct SceneDrawState *out)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    struct SceneDrawState state;

    state.paused = 1;
    state.reset = 0;
    switch (player->unkC)
    {
    case 8:
        switch (player->sceneId)
        {
        case 6:
            state.paused = 0;
            break;
        case 7:
            state.paused = 0;
            break;
        case 8:
        case 10:
            state.paused = 0;
            if (player->frame == 0)
                state.reset = 1;
            break;
        }
        break;
    }
    *out = state;
}

float fn_1_15FA2C(void)
{
    struct ScenePlayer *player = &lbl_1_bss_DD4F4;
    float ret = 1.0f;

    switch (player->unkC)
    {
    case 4:
        switch (player->sceneId)
        {
        case 14:
            if (player->frame < 280)
                ret = 0.5f;
            else if (player->frame < 300)
            {
                ret = 1.0f - 0.05f * (player->frame - 280);
                ret = 0.6f * (1.0f + math_sin(-16384.0f + 65536.0f * ret));
                if (ret > 1.0f)
                    ret = 1.0f;
                ret = 0.5f - 0.25f * ret;
            }
            else
                ret = 0.5f;
            break;
        }
        break;
    }
    return ret;
}

void fn_1_15FB34(struct DialogState *dlg, int a)
{
    dlg->unk0 = 0;
    dlg->unk94 = 0;
    dlg->unk98 = 0;
    fn_1_160B60(dlg->unk10, a);
}

void fn_1_15FB70(struct DialogState *dlg)
{
    fn_1_160D00(dlg->unk10);
}
