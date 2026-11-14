/* Ape animation glue: head tracking and LOD selection. */
#pragma peephole off
#include "types.h"
#include "ball.h"
#include "skeleton.h"
#include "mathutil.h"

extern u32 g_some_other_flags;

void g_something_with_freeing_chara_heap_ape(void);
void g_load_ape_gameplay_variant0(int a, int lod);
void fn_1_113190(void);
void g_ape_anim_body(struct Ape *ape);
void draw_ape(struct Ape *ape);
struct ApeFlagInfo *g_get_ape_flag(s32 a);
void mtxa_rotate_y(s16 angle);
void mtxa_rotate_z(s16 angle);

s32 global_ape_lod[8];
Mtx lbl_1_bss_C8A80;
u32 lbl_1_bss_C8AF0[2];
u32 lbl_1_bss_C8AEC;
u32 lbl_1_bss_C8AE8;
u32 lbl_1_bss_C8AE4;
void *lbl_1_bss_C8AC4[8];
void *lbl_1_bss_C8AB4[4];
void *lbl_1_bss_C8AB0;

void fn_1_D1F0C(void) {}

void set_global_ape_LOD_1(void)
{
    global_ape_lod[0] = 1;
}

void fn_1_D1F24(void) {}

void fn_1_D1F28(void)
{
    g_something_with_freeing_chara_heap_ape();
}

int fn_1_D1F48(void)
{
    return 0;
}

void load_ape_gameplay_id_only(int a)
{
    g_load_ape_gameplay_variant0(a, global_ape_lod[0]);
}

void fn_1_D1F7C(void) {}

void fn_1_D1F80(void) {}

void fn_1_D1F84(void) {}

#ifdef __MWERKS__
void fn_1_D1F88(void)
{
    fn_1_113190();
}
#else
/* A register pass-through on PPC: ending.c calls it as
 * (ape, a, b, float, d) and the args reach fn_1_113190 untouched.
 * x86 passes everything on the stack, so forward them explicitly. */
struct Ape;
#define PC_ASM_STR2(x) #x
#define PC_ASM_STR(x) PC_ASM_STR2(x)
/* __USER_LABEL_PREFIX__ is "_" on i686 Windows, empty on x86_64 */
void fn_1_113190_pc(struct Ape *ape, int b, float f, int c, int d)
    __asm__(PC_ASM_STR(__USER_LABEL_PREFIX__) "fn_1_113190");
void fn_1_D1F88(struct Ape *ape, int a, int b, float f, int d)
{
    fn_1_113190_pc(ape, a, f, b, d);
}
#endif

void set_global_LOD(int lod)
{
    global_ape_lod[0] = lod;
}

void fn_1_D1FB8(void) {}

void fn_1_D1FBC(void) {}

void g_ape_anim_head(struct Ape *ape, Vec *target)
{
    Mtx sp70;
    Mtx sp40;
    Vec sp34;
    Vec sp28;
    Quat sp18;
    struct Bone *head;
    struct ApeFlagInfo *flagInfo;
    struct Bone *neck;
    float speed;

    if (ape != NULL && (u32)ape->unk0 != 0)
    {
        flagInfo = g_get_ape_flag(ape->unk0);
        if ((g_some_other_flags & 0xA) || (ape->flags & 8))
            return;
        if (!(flagInfo->flags & 4) && (ape->flags & 0x40))
        {
            Quat sp8 = { 0.0f, 0.0f, -1.0f, 0.0f };
            Quat *q = &sp8;

            ape->unk2A8 = *q;
            return;
        }
        head = &((struct Skeleton *)ape->bodyAnim)->bones[3];
        neck = &((struct Skeleton *)ape->bodyAnim)->bones[2];
        mtx_copy(head->mtx, sp70);
        mtx_copy(neck->mtx, sp40);
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(lbl_1_bss_C8A80);
        mtxa_mult_right(sp40);
        mtxa_rotate_z(0x4000);
        mtxa_rotate_y(0x4000);
        sp34.x = target->x - ape->pos.x;
        sp34.y = target->y - ape->pos.y;
        sp34.z = target->z - ape->pos.z;
        mtxa_rigid_inv_tf_vec(&sp34, &sp34);
        vec_normalize_len(&sp34);
        {
            int behind = sp34.z > 0.8f;
            int tracking = (flagInfo->flags & 4) == 0;

            if (!tracking)
                ape->flags &= ~0x40;
            if (behind || tracking)
            {
                mtxa_push();
                mtxa_from_mtx(lbl_1_bss_C8A80);
                mtxa_mult_right(sp70);
                mtxa_tf_vec_xyz(1.0f, 0.0f, 0.0f, &sp34);
                mtxa_pop();
                mtxa_rigid_inv_tf_vec(&sp34, &sp34);
            }
            else if (sp34.z > -0.09f)
                sp34 = *(Vec *)&ape->unk2A8;
            if (sp34.y > 0.4f)
                sp34.y = 0.4f;
            else if (sp34.y < -0.3f)
                sp34.y = -0.3f;
            {
                float dot = vec_dot_normalized(&sp34, (Vec *)&ape->unk2A8);

                if (dot > 0.996f || (ape->flags & 0x40))
                {
                    *(Vec *)&ape->unk2A8 = sp34;
                    if (tracking)
                        ape->flags |= 0x40;
                }
                else
                {
                    speed = 5.0f;
                    if (tracking)
                        speed = 8.0f;
                    speed *= 0.4f + 0.8f * math_sqrt(1.0f - dot * dot);
                    if (speed > 6.0f)
                        speed = 6.0f;
                    vec_normalize_len(&sp34);
                    mathutil_vec_cross_prod((Vec *)&ape->unk2A8, &sp34, &sp34);
                    vec_normalize_len(&sp34);
                    ape->unk2A8.w = 0.0f;
                    quat_from_axis_angle(&sp18, &sp34, 182.04445f * speed);
                    quat_mult(&ape->unk2A8, &sp18, &ape->unk2A8);
                }
            }
        }
        vec_normalize_len((Vec *)&ape->unk2A8);
        mtxa_tf_vec((Vec *)&ape->unk2A8, &sp34);
        mtxa_from_mtx(lbl_1_bss_C8A80);
        mtxa_mult_right(sp70);
        mtxa_rigid_inv_tf_vec(&sp34, &sp34);
        sp28.x = 1.0f;
        sp28.y = 0.0f;
        sp28.z = 0.0f;
        quat_from_dirs(&sp18, &sp28, &sp34);
        mtxa_from_quat(&sp18);
        mtxa_mult_left(head->mtx);
        mtxa_to_mtx(head->mtx);
    }
}

void g_smth_with_ape_rotation(struct Ape *ape, Vec *arg1)
{
    Mtx sp44;
    Vec sp38 = *arg1;
    Vec sp2C;
    Vec sp20;

    {
        Vec sp14 = { 1.0f, 0.0f, 0.0f };
        Vec *v = &sp14;

        sp2C = *v;
    }
    {
        Vec sp8 = { 0.0f, 0.0f, -1.0f };
        Vec *v = &sp8;

        sp20 = *v;
    }
    sp38.y = 0.0f;
    vec_normalize_len(&sp38);
    if (__fabs(sp38.x) > __fabs(sp38.z))
        quat_from_dirs(&ape->rot, &sp20, &sp38);
    else
    {
        quat_from_dirs(&ape->rot, &sp2C, &sp38);
        mtxa_from_quat(&ape->rot);
        mtxa_to_mtx(sp44);
        mtxa_from_identity();
        mtxa_rotate_y(-0x4000);
        mtxa_mult_right(sp44);
        mtxa_to_quat(&ape->rot);
    }
}

void fn_1_D250C(void) {}

void fn_1_D2510(struct Ape *ape)
{
    g_ape_anim_body(ape);
}

void fn_1_D2530(struct Ape *ape)
{
    draw_ape(ape);
}
