/* Per-animation state machines for the monkey in the ball. */
#include "types.h"
#include "libc.h"
#include "ball.h"
#include "mathutil.h"
#include "effect.h"
#include "rand.h"
#include "stcoli.h"
#include "skeleton.h"
#include "ape_internal.h"

extern s32 g_smth_with_sound;
extern s32 g_player_id_for_sound;

int fn_1_113728(struct Ape *ape);
int fn_1_1136EC(struct Ape *ape);
void fn_1_113410(struct Ape *ape, int b, int c, int d);
void call_SoundReqID_arg_0(u32 id);
void g_something_with_translating_items(int a, Vec *pos, float b);
u32 fn_1_E5CD0(void);
int fn_1_E5DC8(void);
void mtxa_to_quat(Quat *q);

void set_ape_anim(struct Ape *ape, int anim);
void g_some_set_ape_anim(struct Ape *ape, int anim, int param);

#define BALL_OF_APE(ape) (&balls[ape->ballIdx])

void ape_check_hit(struct Ape *ape, struct Ball *ball)
{
    float speed = ball->unk13C;
    int clip;

    if (speed < -0.11 * 0.9)
    {
        if (speed < -0.11 * 1.7)
        {
            call_SoundReqID_arg_0(0x1A);
            clip = 2;
        }
        else if (speed < -0.11 * 1.3)
        {
            call_SoundReqID_arg_0(0x18);
            clip = 1;
        }
        else
        {
            call_SoundReqID_arg_0(0x17);
            clip = 0;
        }
        ape->unk6C = 3;
        ape->unk70 = clip;
        set_ape_anim(ape, 0);
    }
}

void fn_1_13FF3C(struct Ape *ape)
{
    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            fn_1_113410(ape, ape->unk68, ape->unk6C, ape->unk70);
            ape->robStep++;
        }
        break;
    case 1:
        if (fn_1_1136EC(ape))
            set_ape_anim(ape, 1);
        break;
    }
}

static Vec lbl_1_data_6EAF0[] = {
    {0.0f, 0.0f, -0.3f},
    {0.0f, 0.0f, 0.25f},
    {-0.2f, 0.0f, -0.1f},
    {0.2f, 0.0f, -0.1f},
};

static inline int ape_pick_clip(struct Ape *ape, int hi, int lo, float speed)
{
    int i;

    for (i = hi; i > lo; i--)
    {
        if (speed > fn_1_1156F0((struct ApeData *)ape->unk0, 4, i)->unk14)
            break;
    }
    return i;
}

void g_some_ape_anim_func1(struct Ape *ape)
{
    Vec pos;
    struct RaycastHit hit;
    Vec points[4];
    float speed;
    struct Ball *ball = BALL_OF_APE(ape);
    int state = 0;
    u32 i;
    int j;

    switch (ape->robStep)
    {
    case 0:
        if (ape->robParam & 1)
            ape->flags |= 0x20000000;
        ape->robStep++;
    }
    if (ball->unk17C.flags & 1)
    {
        if (ball->unk17C.pos.y < ball->pos.y - 10.0)
            state = 2;
    }
    else if (ball->vel.y < -0.16203703f)
    {
        ape->flags |= 2;
        state = 1;
    }
    if (!(ape->flags & 0x08000000) && state == 0)
    {
        mtxa_push();
        mtxa_from_translate(&ball->pos);
        mtxa_rotate_y(ball->unk92);
        for (i = 0; i < 4; i++)
            mtxa_tf_point(&lbl_1_data_6EAF0[i], &points[i]);
        for (j = 0; j < 4; j++)
        {
            raycast_stage_down(&points[j], &hit, NULL);
            if (!(hit.flags & 1) || hit.pos.y < ball->unk17C.pos.y - 1.0)
            {
                state = j + 3;
                pos.x = hit.pos.x;
                pos.y = ball->pos.y - 1.0;
                pos.z = hit.pos.z;
                g_something_with_translating_items(3, &pos, 0.5f);
                break;
            }
        }
        mtxa_pop();
    }
    switch (state)
    {
    case 0:
        speed = 216.0f * mathutil_vec_len(&ball->vel);
        ape->unk6C = 2;
        for (state = 11; state > 0; state--)
        {
            if (speed > fn_1_1156F0((struct ApeData *)ape->unk0, 2, state)->unk14)
                break;
        }
        ape->unk70 = state;
        if (mathutil_vec_len(&ball->unkC0) < 0.00027777778f && ball->unkCC < 0.00027777778f)
            set_ape_anim(ape, 4);
        break;
    case 2:
        fn_1_113410(ape, 0, 9, 0);
        break;
    case 1:
        fn_1_113410(ape, 0, 5, 0);
        break;
    case 3:
        speed = 216.0f * mathutil_vec_len(&ball->vel);
        ape->unk6C = 4;
        ape->unk70 = ape_pick_clip(ape, 2, 0, speed);
        break;
    case 4:
        speed = 216.0f * mathutil_vec_len(&ball->vel);
        ape->unk6C = 4;
        ape->unk70 = ape_pick_clip(ape, 5, 3, speed);
        break;
    case 5:
        speed = 216.0f * mathutil_vec_len(&ball->vel);
        ape->unk6C = 4;
        ape->unk70 = ape_pick_clip(ape, 11, 9, speed);
        break;
    case 6:
        speed = 216.0f * mathutil_vec_len(&ball->vel);
        ape->unk6C = 4;
        ape->unk70 = ape_pick_clip(ape, 8, 6, speed);
        break;
    }
    ape_check_hit(ape, ball);
}

static s16 lbl_1_data_6EB3C[] = {0, 1};

void fn_1_1404C0(struct Ape *ape)
{
    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            if (ape->robParam & 4)
                fn_1_113410(ape, 1, 1, 0);
            else
            {
                int i = ape->robParam & 1;

                fn_1_113410(ape, 0, 8, lbl_1_data_6EB3C[i]);
            }
            ape->robStep++;
        }
        break;
    case 1:
        break;
    }
}

static s16 lbl_1_data_6EB40[][2] = {{8, 9}, {6, 7}, {4, 5}, {2, 3}, {0, 1}};

void fn_1_140578(struct Ape *ape)
{
    int i;

    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            if (ape->robParam & 8)
            {
                fn_1_113410(ape, 1, 0, 0);
                ape->unk80 = 1;
            }
            else
            {
                i = ape->robParam & 7;
                fn_1_113410(ape, 0, 7, lbl_1_data_6EB40[i][0]);
                ape->unk80 = lbl_1_data_6EB40[i][1];
            }
            ape->robStep++;
        }
        break;
    case 1:
        if (fn_1_1136EC(ape))
        {
            fn_1_113410(ape, ape->unk74, ape->unk78, ape->unk80);
            ape->unk80 = -1;
            ape->robStep++;
        }
        break;
    case 2:
        ape->flags |= 0x4000;
        ape->robStep++;
        break;
    case 3:
        mtxa_from_quat(&ape->rot);
        mtxa_rotate_y(0xE38);
        mtxa_to_quat(&ape->rot);
        break;
    }
}

void fn_1_1406DC(struct Ape *ape)
{
    struct Ball *ball = BALL_OF_APE(ape);

    ape->flags |= 1;
    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            fn_1_113410(ape, 0, 1, 0);
            ape->robStep++;
            ape->unk82 = 0;
        }
        break;
    case 1:
        if (fn_1_1136EC(ape))
            ape->unk82++;
        if (ape->unk82 >= 5)
        {
            fn_1_113410(ape, 0, 1, 1);
            ape->robStep++;
        }
        break;
    case 2:
        if (fn_1_1136EC(ape))
        {
            fn_1_113410(ape, 0, 1, 2);
            ape->unk82 = 0;
            ape->robStep++;
        }
        break;
    case 3:
        if (fn_1_1136EC(ape))
            ape->unk82++;
        if (ape->unk82 >= 4)
        {
            fn_1_113410(ape, 0, 1, 3);
            ape->robStep++;
        }
        break;
    case 4:
        if (fn_1_1136EC(ape))
        {
            fn_1_113410(ape, 0, 1, 4);
            ape->unk82 = 0;
            ape->robStep++;
        }
        break;
    case 5:
        if (fn_1_1136EC(ape))
            ape->unk82++;
        if (ape->unk82 >= 4)
        {
            fn_1_113410(ape, 0, 1, 5);
            ape->robStep++;
        }
        break;
    case 6:
        if (fn_1_1136EC(ape))
        {
            fn_1_113410(ape, 0, 1, 6);
            ape->robStep++;
        }
        break;
    case 7:
        break;
    }
    if (mathutil_vec_len(&ball->unkC0) > 0.00037037037f)
        g_some_set_ape_anim(ape, 1, 1);
    ape_check_hit(ape, ball);
}

void fn_1_1409D8(struct Ape *ape)
{
    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            fn_1_113410(ape, 0, 0, 0);
            ape->robStep++;
        }
        break;
    case 1:
        if (fn_1_1136EC(ape))
            set_ape_anim(ape, 4);
        break;
    }
}

void fn_1_140A70(struct Ape *ape)
{
    int clip;

    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            switch (ape->robParam)
            {
            case 0:
            default:
                clip = 0;
                break;
            case 1:
                clip = 1;
                break;
            case 2:
                clip = 2;
                break;
            }
            fn_1_113410(ape, 1, 2, clip);
            ape->robStep++;
        }
        break;
    case 1:
        break;
    }
}

void fn_1_140B1C(struct Ape *ape)
{
    struct Effect effect;
    float cur;
    float prev;
    float delta;
    int clip;
    u32 mask;
    int crossed;
    int pan;
    int vol;

    if (ape == NULL)
        return;
    switch (ape->robStep)
    {
    case 0:
        if (fn_1_113728(ape))
        {
            switch (ape->robParam)
            {
            case 0:
            default:
                clip = 0;
                ape->robStep = 1;
                break;
            case 1:
                if (rand() & 1)
                    clip = 3;
                else
                    clip = 4;
                ape->robStep = 3;
                break;
            case 2:
                if (rand() & 1)
                    clip = 5;
                else
                    clip = 6;
                ape->robStep = 3;
                break;
            case 3:
                if (rand() & 1)
                    clip = 7;
                else
                    clip = 8;
                ape->robStep = 3;
                break;
            }
            fn_1_113410(ape, 0x1A, 0, clip);
            ape->unk82 = 0;
        }
        break;
    case 1:
        if (fn_1_E5DC8() && fn_1_1136EC(ape))
            ape->unk82++;
        if (ape->unk82 >= 8)
        {
            fn_1_113410(ape, 0x1A, 0, 1);
            ape->robStep++;
        }
        break;
    case 2:
        switch (ape->charaId)
        {
        case 0:
            if (fn_1_1135E8(ape) >= 150.0)
                ape->flags |= 0x02000000;
            break;
        case 3:
            cur = fn_1_1135E8(ape);
            delta = body_frame_add72(ape);
            if (186.0 - 0.5 * delta <= cur && cur < 186.0 + 0.5 * delta)
            {
                memset(&effect, 0, sizeof(effect));
                effect.type = 0x17;
                effect.playerId = ape->apeIdx;
                effect.u_otherTimer = ape->unk88;
                spawn_effect(&effect);
            }
            break;
        }
        if (fn_1_1136EC(ape))
        {
            fn_1_113410(ape, 0x1A, 0, 2);
            ape->robStep++;
        }
        break;
    case 3:
        break;
    }
    mask = fn_1_E5CD0();
    if (mask == (1 << ((ape->unk29C >> 8) & 0xFF))
     || ((mask & (1 << ((ape->unk29C >> 8) & 0xFF))) && (rand() & 0x7FFF) % 3 == 0))
    {
        crossed = 0;
        prev = body_frame_add72(ape);
        if (fn_1_1135E8(ape) - 1.0 < prev)
            crossed = 1;
        if (crossed)
        {
            pan = 0;
            vol = 0;
            g_smth_with_sound = ape->charaId;
            g_player_id_for_sound = 0;
            switch (ape->unk29C & 0xFF)
            {
            case 0:
                call_SoundReqID_arg_0(0x1B | (((s8)pan & 0x7F) << 18) | (((s8)vol & 0x7F) << 25));
                break;
            case 1:
                call_SoundReqID_arg_0(0x58 | (((s8)pan & 0x7F) << 18) | (((s8)vol & 0x7F) << 25));
                break;
            case 2:
                call_SoundReqID_arg_0(0x3F | (((s8)pan & 0x7F) << 18) | (((s8)vol & 0x7F) << 25));
                break;
            case 3:
                call_SoundReqID_arg_0(0x1C | (((s8)pan & 0x7F) << 18) | (((s8)vol & 0x7F) << 25));
                break;
            }
        }
    }
}

static s8 lbl_1_data_6EB74[] = {0x11, 3, 0, 0x11, 3, 1, 0x11, 3, 2};
static s8 lbl_1_data_6EB80[] = {0x11, 4, 0, 0x11, 4, 1, 0x11, 4, 2};
static s8 lbl_1_data_6EB8C[] = {0x11, 5, 0, 0x11, 5, 0, 0x11, 5, 1};
static s8 *lbl_1_data_6EB98[] = {lbl_1_data_6EB74, lbl_1_data_6EB80, lbl_1_data_6EB8C};
static s8 lbl_1_data_6EBA4[] = {0x15, 0xB, 0, 0x15, 0xB, 0, 0x15, 0xC, 0};
static s8 lbl_1_data_6EBB0[] = {0x15, 0xB, 1, 0x15, 0xB, 1, 0x15, 0xC, 1};
static s8 lbl_1_data_6EBBC[] = {0x15, 0xB, 2, 0x15, 0xB, 2, 0x15, 0xC, 2};
static s8 lbl_1_data_6EBC8[] = {0x15, 0xB, 3, 0x15, 0xB, 3, 0x15, 0xC, 5};
static s8 lbl_1_data_6EBD4[] = {0x15, 0xB, 4, 0x15, 0xB, 4, 0x15, 0xC, 5};
static s8 lbl_1_data_6EBE0[] = {0x15, 0xB, 5, 0x15, 0xB, 5, 0x15, 0xC, 3};
static s8 lbl_1_data_6EBEC[] = {0x15, 0xB, 6, 0x15, 0xB, 6, 0x15, 0xC, 6};
static s8 lbl_1_data_6EBF8[] = {0x15, 0xB, 7, 0x15, 0xB, 7, 0x15, 0xC, 4};
static s8 *lbl_1_data_6EC04[] = {
    lbl_1_data_6EBA4, lbl_1_data_6EBB0, lbl_1_data_6EBBC, lbl_1_data_6EBC8,
    lbl_1_data_6EBD4, lbl_1_data_6EBE0, lbl_1_data_6EBEC, lbl_1_data_6EBF8,
};
static s8 **lbl_1_data_6EC24[] = {lbl_1_data_6EB98, lbl_1_data_6EC04};

void fn_1_140F48(struct Ape *ape)
{
    int group = (ape->robParam & 0x38) >> 3;
    int idx = ape->robParam & 7;
    s8 *p = lbl_1_data_6EC24[group][idx];

    if (ape->robParam & 0x40)
    {
        p += 6;
        fn_1_113410(ape, p[0], p[1], p[2]);
        ape->robStep = 3;
    }
    else
    {
        switch (ape->robStep)
        {
        case 0:
            if (fn_1_113728(ape))
            {
                fn_1_113410(ape, p[0], p[1], p[2]);
                ape->robStep++;
            }
            break;
        case 1:
            if (fn_1_1136EC(ape))
            {
                p += 3;
                fn_1_113410(ape, p[0], p[1], p[2]);
                ape->robStep++;
            }
            break;
        case 2:
        {
            float frame = fn_1_1135E8(ape);

            if (142.0 <= frame && frame <= 740.0)
                ape->robParam |= 0x80;
            else
                ape->robParam &= ~0x80;
            if (fn_1_1136EC(ape))
            {
                fn_1_113410(ape, p[0], p[1], p[2]);
                ape->robStep = 1;
            }
            break;
        }
        case 3:
            if (fn_1_1136EC(ape))
            {
                fn_1_113410(ape, p[0], p[1], p[2]);
                ape->robStep = 1;
            }
            break;
        }
    }
}

void set_ape_anim(struct Ape *ape, int anim)
{
    if (ape != NULL)
    {
        ape->unk86 = (s8)anim;
        ape->robParam = 0;
        ape->robStep = 0;
        ape->unk80 = -1;
    }
}

void g_some_set_ape_anim(struct Ape *ape, int anim, int param)
{
    if (ape != NULL)
    {
        ape->unk86 = (s8)anim;
        ape->robParam = (s16)param;
        ape->robStep = 0;
        ape->unk80 = -1;
    }
}

void (*g_some_chara_anim_funcs[])(struct Ape *) = {
    fn_1_13FF3C,
    g_some_ape_anim_func1,
    fn_1_1404C0,
    fn_1_140578,
    fn_1_1406DC,
    fn_1_1409D8,
    fn_1_140A70,
    fn_1_140B1C,
    fn_1_140F48,
};

void run_anim_funcs(struct Ape *ape)
{
    g_some_chara_anim_funcs[ape->unk86](ape);
}
