/* Skeletal animation for the apes: a Skeleton owns an array of Bones, plays
 * clips from an MTA archive and blends between them. */
#pragma peephole off
#include "types.h"
#include "libc.h"
#include "mathutil.h"
#include "skeleton.h"

int fn_1_10D0E0(float frame, Vec *dst, struct Bone *bone, u16 channel, int prev);
void fn_1_10CB70(struct Skeleton *skel, struct Bone *bone, u16 a, u16 b, float t);
void fn_1_10D7F8(struct Skeleton *skel, struct Bone *bone);
void ape_archive_offsets_to_pointers(u32 *archive);
int g_table_index();
void quat_slerp(Quat *out, Quat *a, Quat *b, float t);
void ray_scale(Vec *a, Vec *b, Vec *out, float t);
void mtxa_to_quat(Quat *q);
void mtx_from_identity(Mtx m);

extern float lbl_80198AC0;  /* NaN */
extern float lbl_80198AC4;  /* infinity */


void g_init_bone_state_subA(struct AnimClock *clock);
void g_init_skeleton_animation(struct Skeleton *skel, struct AnimInfo *info, u16 clipIdx);
void g_set_more_ape_state(struct AnimClock *dst, struct AnimClock *src);
void g_smth_with_quat_slerp(struct Skeleton *skel);
void fn_1_10C598(struct Skeleton *skel, struct Bone *bone, int prev);
void fn_1_10BF98(struct Skeleton *skel);
void g_unk_ape_anim2(struct AnimClock *clock, float speed);

void init_component_state(struct Skeleton *skel, struct Bone *bones, u8 *data)
{
    u8 *p;
    u32 i;

    skel->speed = 1.0f;
    skel->boneCount = *(u16 *)data;
    skel->bones = bones;
    skel->scaleX = 1.0f;
    skel->scaleY = 1.0f;
    skel->scaleZ = 1.0f;
    skel->mta = NULL;
    skel->flags = 0;
    skel->blendFrame = 0.0f;
    skel->blendLength = 0.0f;
    skel->unk18.x = 0.0f;
    skel->unk18.y = 0.0f;
    skel->unk18.z = 0.0f;
    skel->unk30.x = 0.0f;
    skel->unk30.y = 0.0f;
    skel->unk30.z = 0.0f;
    g_init_bone_state_subA(&skel->clock);
    g_init_bone_state_subA(&skel->prevClock);
    p = data + 4;
    for (i = 0; i < skel->boneCount; i++)
    {
        u32 len;
        Vec *scale;

#ifdef __MWERKS__
        skel->bones[i].parent = *((u16 *)p)++;
        skel->bones[i].flags = *((u16 *)p)++;
        mtxa_from_quat(((Quat *)p)++);
        mtxa_to_mtx(skel->bones[i].unk28);
        mtxa_from_quat(((Quat *)p)++);
        mtxa_to_mtx(skel->bones[i].unk58);
        skel->bones[i].pos.x = *((float *)p)++;
        skel->bones[i].pos.y = *((float *)p)++;
        skel->bones[i].pos.z = *((float *)p)++;
#else
        skel->bones[i].parent = *(u16 *)p; p += sizeof(u16);
        skel->bones[i].flags = *(u16 *)p; p += sizeof(u16);
        mtxa_from_quat((Quat *)p); p += sizeof(Quat);
        mtxa_to_mtx(skel->bones[i].unk28);
        mtxa_from_quat((Quat *)p); p += sizeof(Quat);
        mtxa_to_mtx(skel->bones[i].unk58);
        skel->bones[i].pos.x = *(float *)p; p += sizeof(float);
        skel->bones[i].pos.y = *(float *)p; p += sizeof(float);
        skel->bones[i].pos.z = *(float *)p; p += sizeof(float);
#endif
        skel->bones[i].len = mathutil_vec_len(&skel->bones[i].pos);
        skel->bones[i].unk18 = skel->bones[i].pos;
        skel->bones[i].unk24 = 1.0f;
        skel->bones[i].name = (char *)p;
        skel->bones[i].handle = NULL;
        scale = &skel->bones[i].scale;
        scale->x = 1.0f;
        scale->y = 1.0f;
        scale->z = 1.0f;
        mtx_from_identity(skel->bones[i].mtx);
        len = strlen((char *)p);
        len += 3 - (len & 3);
        p = (u8 *)((u32)p + len + 1);
    }
}

void g_init_some_handl_component_stuff(struct Skeleton *skel, struct BoneHandle *handles)
{
    int i;

    skel->handles = handles;
    for (i = 0; i < skel->boneCount; i++)
        skel->bones[i].handle = &handles[i];
}

void g_init_mta(struct Skeleton *skel, struct Mta *mta)
{
    skel->mta = mta;
    ape_archive_offsets_to_pointers((u32 *)mta);
}

void g_some_ape_anim_init_wrapper(struct Skeleton *skel)
{
    u32 idx = g_table_index(skel->mta);

    if (idx == -1)
        idx = 0;
    g_init_skeleton_animation(skel, NULL, (u16)idx);
}

void g_init_skeleton_animation(struct Skeleton *skel, struct AnimInfo *info, u16 clipIdx)
{
    struct MtaClip *c = skel->mta->clips[clipIdx];
    u8 *clip = (u8 *)c;
    u32 count;
    u32 i;
    int j;
    u16 chanFlags;

    skel->blendFrame = 0.0f;
    skel->blendLength = 0.0f;
    skel->flags &= 10;
    skel->unk30.x = 0.0f;
    skel->unk30.y = 0.0f;
    skel->unk30.z = 0.0f;
    if (info != NULL)
    {
        if ((u16)skel->clock.end >= info->end)
            skel->clock.end = info->end;
        else
            skel->clock.end = ((struct MtaClip *)clip)->length;
        skel->clock.loopStart = info->loopStart;
        skel->clock.unk6 = info->unk6;
        skel->clock.loops = 0;
    }
    else
    {
        skel->clock.end = ((struct MtaClip *)clip)->length;
        skel->clock.loopStart = 1;
        skel->clock.unk6 = 1;
        skel->clock.loops = 0;
    }
    skel->clock.flags = 0;
    skel->clock.length = ((struct MtaClip *)clip)->length;
    skel->clock.frame = skel->clock.loopStart;
    count = (u16)skel->boneCount < ((struct MtaClip *)clip)->boneCount
          ? (u32)skel->boneCount : (u32)((struct MtaClip *)clip)->boneCount;
    clip += 4;
    for (i = 0; i < count; i++)
    {
        chanFlags = *(u16 *)clip;
        clip += 2;
        clip += 2;
        for (j = 0; j < 3; j++)
        {
            if (chanFlags & 1)
            {
#ifdef __MWERKS__
                skel->bones[i].keyCount[j] = *((u16 *)clip)++;
#else
                skel->bones[i].keyCount[j] = *(u16 *)clip; clip += 2;
#endif
                clip += 2;
                skel->bones[i].keys[j] = clip;
                clip += skel->bones[i].keyCount[j] << 4;
            }
            else
            {
                skel->bones[i].keyCount[j] = 0;
                skel->bones[i].keys[j] = NULL;
            }
            skel->bones[i].keyIdx[j] = 0;
            chanFlags >>= 1;
        }
        for (j = 0; j < 3; j++)
        {
            if (chanFlags & 1)
            {
#ifdef __MWERKS__
                skel->bones[i].keyCount[j + 6] = *((u16 *)clip)++;
#else
                skel->bones[i].keyCount[j + 6] = *(u16 *)clip; clip += 2;
#endif
                clip += 2;
                skel->bones[i].keys[j + 6] = clip;
                clip += skel->bones[i].keyCount[j + 6] << 4;
            }
            else
            {
                skel->bones[i].keyCount[j + 6] = 0;
                skel->bones[i].keys[j + 6] = NULL;
            }
            skel->bones[i].keyIdx[j + 6] = 0;
            chanFlags >>= 1;
        }
        for (j = 0; j < 3; j++)
        {
            if (chanFlags & 1)
            {
#ifdef __MWERKS__
                skel->bones[i].keyCount[j + 12] = *((u16 *)clip)++;
#else
                skel->bones[i].keyCount[j + 12] = *(u16 *)clip; clip += 2;
#endif
                clip += 2;
                skel->bones[i].keys[j + 12] = clip;
                clip += skel->bones[i].keyCount[j + 12] << 4;
            }
            else
            {
                skel->bones[i].keyCount[j + 12] = 0;
                skel->bones[i].keys[j + 12] = NULL;
            }
            skel->bones[i].keyIdx[j + 12] = 0;
            chanFlags >>= 1;
        }
    }
}

void g_unk_ape_anim1(struct Skeleton *skel, struct AnimInfo *info, u16 clipIdx, u16 blendLength, u16 flags)
{
    float t;
    float range;
    int i;

    if (blendLength == 0)
    {
        g_init_skeleton_animation(skel, info, clipIdx);
    }
    else
    {
        float loopStart = skel->clock.loopStart;
        float newRange;

        range = (float)skel->clock.end - loopStart;
        t = (skel->clock.frame - loopStart) / range;
        g_set_more_ape_state(&skel->prevClock, &skel->clock);
        if (flags & 2)
        {
            for (i = 0; i < skel->boneCount; i++)
            {
                struct Bone *bone = &skel->bones[i];
                int j;

                for (j = 0; j < 3; j++)
                {
                    bone->keyCount[j + 3] = bone->keyCount[j];
                    bone->keyCount[j + 9] = bone->keyCount[j + 6];
                    bone->keyCount[j + 15] = bone->keyCount[j + 12];
                    bone->keys[j + 3] = bone->keys[j];
                    bone->keys[j + 9] = bone->keys[j + 6];
                    bone->keys[j + 15] = bone->keys[j + 12];
                    bone->keyIdx[j + 3] = bone->keyIdx[j];
                    bone->keyIdx[j + 9] = bone->keyIdx[j + 6];
                    bone->keyIdx[j + 15] = bone->keyIdx[j + 12];
                }
            }
        }
        else
        {
            for (i = 0; i < skel->boneCount; i++)
            {
                struct Bone *bone = &skel->bones[i];

                mtxa_from_mtx(bone->mtx);
                mtxa_to_quat(&bone->quat);
            }
        }
        g_init_skeleton_animation(skel, info, clipIdx);
        newRange = skel->clock.end - skel->clock.loopStart;
        if (flags & 2)
        {
            skel->flags |= 1;
            skel->prevClock.speed *= range / newRange;
        }
        if (flags & 1)
            skel->clock.frame = (float)skel->clock.loopStart + newRange * t;
        skel->blendLength = blendLength;
        skel->unk30.x = skel->bones[0].mtx[0][3];
        skel->unk30.y = skel->bones[0].mtx[1][3];
        skel->unk30.z = skel->bones[0].mtx[2][3];
    }
}

void fn_1_10BE3C(struct Skeleton *skel)
{
    int i;
    struct Bone *bone;

    mtxa_push();
    if (skel->flags & 1)
    {
        for (i = 0; i < skel->boneCount; i++)
            fn_1_10C598(skel, &skel->bones[i], 1);
        for (i = 0; i < skel->boneCount; i++)
        {
            bone = &skel->bones[i];
            mtxa_from_mtx(bone->mtx);
            mtxa_to_quat(&bone->quat);
        }
    }
    for (i = 0; i < skel->boneCount; i++)
        fn_1_10C598(skel, &skel->bones[i], 0);
    skel->unk18.x = skel->bones[0].mtx[0][3];
    skel->unk18.y = skel->bones[0].mtx[1][3];
    skel->unk18.z = skel->bones[0].mtx[2][3];
    g_smth_with_quat_slerp(skel);
    mtxa_pop();
    if (!(skel->flags & 0x10))
        fn_1_10BF98(skel);
    if (skel->clock.flags & 2)
        skel->flags |= 4;
    else
        skel->flags &= ~4;
}

void fn_1_10BF98(struct Skeleton *skel)
{
    g_unk_ape_anim2(&skel->clock, skel->speed);
    if (skel->flags & 1)
        g_unk_ape_anim2(&skel->prevClock, skel->speed);
}

void fn_1_10BFE8(struct Skeleton *skel, u16 a, u16 b, float t)
{
    int i;

    mtxa_push();
    for (i = 0; i < skel->boneCount; i++)
        fn_1_10CB70(skel, &skel->bones[i], a, b, t);
    skel->unk18.x = skel->bones[0].mtx[0][3];
    skel->unk18.y = skel->bones[0].mtx[1][3];
    skel->unk18.z = skel->bones[0].mtx[2][3];
    g_smth_with_quat_slerp(skel);
    mtxa_pop();
}

void g_init_bone_state_subA(struct AnimClock *clock)
{
    clock->flags = 0;
    clock->speed = 1.0f;
    clock->frame = 0.0f;
    clock->length = 0.0f;
    clock->loopStart = 0;
    clock->end = 0;
    clock->unk6 = 0;
}

void g_unk_ape_anim2(struct AnimClock *clock, float speed)
{
    float start;
    float end;

    if (clock->flags & 2)
        return;
    speed *= clock->speed;
    start = clock->loopStart;
    end = clock->end;
    if ((clock->frame += speed) > end)
    {
        if (clock->flags & 1)
        {
            clock->frame = end;
            clock->flags |= 2;
        }
        else
        {
            start = clock->unk6;
            clock->loopStart = start;
            clock->frame -= end + speed - start;
            clock->loops++;
        }
    }
    else if (clock->frame < start)
    {
        if (clock->flags & 1)
        {
            clock->frame = start;
            clock->flags |= 2;
        }
        else
        {
            clock->frame += end - speed - start;
            clock->loopStart = clock->unk6;
            clock->loops++;
        }
    }
}

void g_set_more_ape_state(struct AnimClock *dst, struct AnimClock *src)
{
    dst->flags = src->flags;
    dst->frame = src->frame;
    dst->length = src->length;
    dst->speed = src->speed;
    dst->loopStart = src->loopStart;
    dst->unk6 = src->unk6;
    dst->end = src->end;
}

void g_smth_with_quat_slerp(struct Skeleton *skel)
{
    Vec pos;
    Quat q;
    float t;
    struct Bone *bones;
    int i;
    struct Bone *bone;

    if (skel->blendFrame < skel->blendLength)
    {
        t = skel->blendFrame / skel->blendLength;
        skel->blendFrame += 1.0f;
    }
    else
    {
        t = 1.0f;
        skel->blendFrame = 0.0f;
        skel->blendLength = 0.0f;
        skel->flags &= ~1;
    }
    bones = skel->bones;
    for (i = 0; i < skel->boneCount; i++)
    {
        bone = &bones[i];
        if (i == 0)
        {
            pos.x = skel->unk18.x * skel->scaleX;
            pos.y = skel->unk18.y * skel->scaleY;
            pos.z = skel->unk18.z * skel->scaleZ;
            pos.x = skel->unk30.x + t * (pos.x - skel->unk30.x);
            pos.y = skel->unk30.y + t * (pos.y - skel->unk30.y);
            pos.z = skel->unk30.z + t * (pos.z - skel->unk30.z);
        }
        else
        {
            if (bone->parent == 0xFFFF)
                mtxa_from_identity();
            else
                { struct Bone *p = &bones[bone->parent]; mtxa_from_mtx(p->mtx); }
            if (bone->flags & 0x11)
            {
                pos.x = bone->len * skel->scaleX;
                pos.y = 0.0f;
                pos.z = 0.0f;
                if (skel->flags & 8)
                    pos.x *= bone->unk24;
            }
            else if (skel->flags & 8)
                pos = bone->unk18;
            else
                pos = bone->unkC4;
            mtxa_tf_point(&pos, &pos);
        }
        mtxa_from_mtx(bone->mtx);
        bone->scale.x *= skel->scaleX;
        bone->scale.y *= skel->scaleY;
        bone->scale.z *= skel->scaleZ;
        if (t < 1.0f)
        {
            mtxa_to_quat(&q);
            quat_slerp(&q, &bone->quat, &q, t);
            mtxa_from_quat(&q);
        }
        mathutil_mtxA_set_translate(&pos);
        mtxa_scale_xyz(bone->scale.x, bone->scale.y, bone->scale.z);
        mtxa_to_mtx(bone->mtx);
    }
}

void fn_1_10C598(struct Skeleton *skel, struct Bone *bone, int prev)
{
    Vec parentScale;
    Vec rot;
    Vec pos;
    int chan;
    struct Bone *src;
    float frame;

    src = bone;
    if (prev != 0)
    {
        chan = 3;
        frame = skel->prevClock.frame;
    }
    else
    {
        chan = 0;
        frame = skel->clock.frame;
    }
    if (bone->parent == 0xFFFF)
    {
        mtxa_from_identity();
        parentScale.x = 1.0f;
        parentScale.y = 1.0f;
        parentScale.z = 1.0f;
    }
    else
    {
        struct Bone *parent = &skel->bones[bone->parent];

        mtxa_from_mtx(parent->mtx);
        parentScale = parent->scale;
    }
    if ((skel->flags & 2) && !(bone->handle->flags & 0x40))
        src = &skel->bones[bone->handle->mirrorBone];
    if (bone->flags & 1)
    {
        mtxa_tf_point(&bone->pos, &pos);
        fn_1_10D0E0(frame, &pos, src, chan + 0xC, prev);
        if (skel->flags & 2)
        {
            if (bone->handle->flags & 1)
                pos.x = -pos.x;
            if (bone->handle->flags & 2)
                pos.y = -pos.y;
            if (bone->handle->flags & 4)
                pos.z = -pos.z;
        }
    }
    else
    {
        pos = bone->pos;
        fn_1_10D0E0(frame, &pos, src, chan + 0xC, prev);
        if (skel->flags & 2)
        {
            if (bone->handle->flags & 1)
                pos.x = -pos.x;
            if (bone->handle->flags & 2)
                pos.y = -pos.y;
            if (bone->handle->flags & 4)
                pos.z = -pos.z;
        }
        mtxa_tf_point(&pos, &pos);
    }
    mathutil_mtxA_set_translate(&pos);
    mathutil_mtxA_get_translate(&bone->unkC4);
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;
    mtxa_mult_right(bone->unk28);
    if (skel->flags & 2)
    {
        mtxa_rotate_z(182.04445f * (bone->handle->rotZ * 45));
        mtxa_rotate_y(182.04445f * (bone->handle->rotY * 45));
        mtxa_rotate_x(182.04445f * (bone->handle->rotX * 45));
    }
    if (fn_1_10D0E0(frame, &rot, src, chan + 6, prev) != 0)
    {
        if (skel->flags & 2)
        {
            if (bone->handle->flags & 8)
                rot.x = -rot.x;
            if (bone->handle->flags & 0x10)
                rot.y = -rot.y;
            if (bone->handle->flags & 0x20)
                rot.z = -rot.z;
        }
        mtxa_rotate_z(182.04445f * rot.z);
        mtxa_rotate_y(182.04445f * rot.y);
        mtxa_rotate_x(182.04445f * rot.x);
    }
    else
        mtxa_mult_right(bone->unk58);
    bone->scale.x = 1.0f;
    bone->scale.y = 1.0f;
    bone->scale.z = 1.0f;
    fn_1_10D0E0(frame, &bone->scale, src, chan, prev);
    bone->scale.x *= parentScale.x;
    bone->scale.y *= parentScale.y;
    bone->scale.z *= parentScale.z;
    mtxa_to_mtx(bone->mtx);
    if (bone->flags & 1)
        fn_1_10D7F8(skel, bone);
    else if (bone->parent != 0xFFFF)
    {
        mtxa_from_mtx(skel->bones[bone->parent].mtx);
        mtxa_rigid_inv_tf_point(&bone->unkC4, &bone->unkC4);
    }
}

void fn_1_10CB70(struct Skeleton *skel, struct Bone *bone, u16 a, u16 b, float t)
{
    Vec parentScale;
    Vec rot;
    Vec scale;
    Vec pos;
    Vec posA;

    if (bone->parent == 0xFFFF)
    {
        mtxa_from_identity();
        parentScale.x = 1.0f;
        parentScale.y = 1.0f;
        parentScale.z = 1.0f;
    }
    else if (bone->flags & 4)
    {
        mtxa_sq_from_identity();
        parentScale.x = 1.0f;
        parentScale.y = 1.0f;
        parentScale.z = 1.0f;
    }
    else
    {
        struct Bone *parent = &skel->bones[bone->parent];

        mtxa_from_mtx(parent->mtx);
        parentScale = parent->scale;
    }
    if (bone->flags & 1)
    {
        mtxa_tf_point(&bone->pos, &pos);
        posA = pos;
        fn_1_10D0E0((float)a, &posA, bone, 0xC, 0);
        fn_1_10D0E0((float)b, &pos, bone, 0xC, 0);
        ray_scale(&posA, &pos, &pos, t);
    }
    else
    {
        pos = bone->pos;
        posA = pos;
        fn_1_10D0E0((float)a, &posA, bone, 0xC, 0);
        fn_1_10D0E0((float)b, &pos, bone, 0xC, 0);
        ray_scale(&posA, &pos, &pos, t);
        mtxa_tf_point(&pos, &pos);
    }
    mathutil_mtxA_set_translate(&pos);
    mathutil_mtxA_get_translate(&bone->unkC4);
    mtxa_mult_right(bone->unk28);
    if (fn_1_10D0E0((float)b, &rot, bone, 6, 0) != 0)
    {
        fn_1_10D0E0((float)a, &posA, bone, 6, 0);
        ray_scale(&posA, &rot, &rot, t);
        if (skel->flags & 2)
        {
            rot.x = -rot.x;
            rot.y = -rot.y;
        }
        mtxa_rotate_z(182.04445f * rot.z);
        mtxa_rotate_y(182.04445f * rot.y);
        mtxa_rotate_x(182.04445f * rot.x);
    }
    else
        mtxa_mult_right(bone->unk58);
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
    bone->scale.x = 1.0f;
    bone->scale.y = 1.0f;
    bone->scale.z = 1.0f;
    fn_1_10D0E0((float)a, &scale, bone, 0, 0);
    fn_1_10D0E0((float)b, &bone->scale, bone, 0, 0);
    ray_scale(&scale, &bone->scale, &bone->scale, t);
    bone->scale.x *= parentScale.x;
    bone->scale.y *= parentScale.y;
    bone->scale.z *= parentScale.z;
    if (bone->flags & 1)
        fn_1_10D7F8(skel, bone);
    else
    {
        mtxa_to_mtx(bone->mtx);
        if (bone->parent != 0xFFFF)
        {
            mtxa_from_mtx(skel->bones[bone->parent].mtx);
            mtxa_rigid_inv_tf_point(&bone->unkC4, &bone->unkC4);
        }
    }
}

static inline float sample_keys(float frame, int count, struct BoneKey *keys)
{
    float t, t2, t3, dur, value, next, tanOut, tanIn;
    int i;

    if (frame < keys[0].frame)
        return keys[0].value;
    if (frame >= keys[count - 1].frame)
        return keys[count - 1].value;
    for (i = 0; frame >= keys[i + 1].frame; i++)
        ;
    keys += i;
    value = keys[0].value;
    next = keys[1].value;
    tanOut = keys[0].tanOut;
    tanIn = keys[1].tanIn;
    dur = keys[1].frame - keys[0].frame;
    t = (frame - keys[0].frame) / dur;
    switch (keys->type & 3)
    {
    case 1:
        value += t * (next - value);
        return value;
    case 0:
        t2 = t * t;
        t3 = t2 * t;
        value = value * (1.0f + (2.0f * t3 - 3.0f * t2)) + next * (-2.0f * t3 + 3.0f * t2);
        value += 0.033333335f * dur * (tanOut * (t + (t3 - 2.0f * t2)) + tanIn * (t3 - t2));
        return value;
    }
    return value;
}

int fn_1_10D0E0(float frame, Vec *dst, struct Bone *bone, u16 channel, int prev)
{
    int result = 0;

    if (bone->keyCount[channel] != 0)
    {
        dst->x = sample_keys(frame, bone->keyCount[channel], bone->keys[channel]);
        result = 1;
    }
    channel++;
    if (bone->keyCount[channel] != 0)
    {
        dst->y = sample_keys(frame, bone->keyCount[channel], bone->keys[channel]);
        result = 1;
    }
    channel++;
    if (bone->keyCount[channel] != 0)
    {
        dst->z = sample_keys(frame, bone->keyCount[channel], bone->keys[channel]);
        result = 1;
    }
    return result;
}

static inline void vec_cross(register Vec *a, register Vec *b, register Vec *out)
{
#ifdef __MWERKS__
    register float ax, ay, az, bx, by, bz, cx, cy, cz;

    asm
    {
        lfs ay, 4(a)
        lfs bz, 8(b)
        lfs az, 8(a)
        lfs bx, 0(b)
        lfs ax, 0(a)
        lfs by, 4(b)
        fmuls cx, ay, bz
        fmuls cy, az, bx
        fmuls cz, ax, by
        fnmsubs cx, az, by, cx
        stfs cx, 0(out)
        fnmsubs cy, ax, bz, cy
        stfs cy, 4(out)
        fnmsubs cz, ay, bx, cz
        stfs cz, 8(out)
    }
#else
    /* PC: the asm above stores each component right after computing it,
     * after all inputs are loaded, so aliasing out with a/b is safe. */
    float ax = a->x, ay = a->y, az = a->z;
    float bx = b->x, by = b->y, bz = b->z;

    out->x = ay * bz - az * by;
    out->y = az * bx - ax * bz;
    out->z = ax * by - ay * bx;
#endif
}

void fn_1_10D7F8(struct Skeleton *skel, struct Bone *bone)
{
    struct Bone *chain[3];
    float dist;
    int n;
    struct Bone *bones;
    struct Bone *p;

    bones = skel->bones;
    n = 0;
    p = &bones[bone->parent];
    while (!(p->flags & 2))
    {
        if (n == 2 || p->parent == -1)
            break;
        chain[n++] = p;
        p = &bones[p->parent];
    }
    if (n == 0 || (n == 1 && chain[0]->len > 1.1920929e-7f))
        chain[n++] = p;
    {
        Vec dir;
        Vec v5C;
        Vec joint;
        Vec v44;
        Vec v38;
        Vec v2C;
        Vec v20;
        Vec v14;
        Vec v8;

    mtxa_from_mtx(p->mtx);
    dir.x = bone->mtx[0][3] - mtxa->mtxA[0][3];
    dir.y = bone->mtx[1][3] - mtxa->mtxA[1][3];
    dir.z = bone->mtx[2][3] - mtxa->mtxA[2][3];
    dist = mathutil_vec_len(&dir);
    vec_normalize_len(&dir);
    v44.x = mtxa->mtxA[0][2];
    v44.y = mtxa->mtxA[1][2];
    v44.z = mtxa->mtxA[2][2];
    vec_cross(&v44, &dir, &v38);
    vec_normalize_len(&v38);
    vec_cross(&dir, &v38, &v44);
    mtxa->mtxA[0][0] = dir.x;
    mtxa->mtxA[1][0] = dir.y;
    mtxa->mtxA[2][0] = dir.z;
    mtxa->mtxA[0][1] = v38.x;
    mtxa->mtxA[1][1] = v38.y;
    mtxa->mtxA[2][1] = v38.z;
    mtxa->mtxA[0][2] = v44.x;
    mtxa->mtxA[1][2] = v44.y;
    mtxa->mtxA[2][2] = v44.z;
    if (n > 1)
    {
        float len = chain[0]->len;
        float boneLen = bone->len;
        float lenSq = len * len;
        float y;

        dist = (lenSq - boneLen * boneLen + dist * dist) / (2.0f * dist);
        if (len > dist)
            y = math_sqrt(lenSq - dist * dist);
        else
        {
            y = 0.0f;
            dist = len;
        }
        if (!(chain[0]->flags & 8))
            y = -y;
        mtxa_tf_vec_xyz(dist, y, 0.0f, &v5C);
        joint.x = chain[1]->mtx[0][3] + v5C.x;
        joint.y = chain[1]->mtx[1][3] + v5C.y;
        joint.z = chain[1]->mtx[2][3] + v5C.z;
        vec_normalize_len(&v5C);
        v2C.x = mtxa->mtxA[0][2];
        v2C.y = mtxa->mtxA[1][2];
        v2C.z = mtxa->mtxA[2][2];
        vec_cross(&v2C, &v5C, &v20);
        vec_normalize_len(&v20);
        vec_cross(&v5C, &v20, &v2C);
        mtxa->mtxA[0][0] = v5C.x;
        mtxa->mtxA[1][0] = v5C.y;
        mtxa->mtxA[2][0] = v5C.z;
        mtxa->mtxA[0][1] = v20.x;
        mtxa->mtxA[1][1] = v20.y;
        mtxa->mtxA[2][1] = v20.z;
        mtxa->mtxA[0][2] = v2C.x;
        mtxa->mtxA[1][2] = v2C.y;
        mtxa->mtxA[2][2] = v2C.z;
        mtxa_to_mtx(chain[1]->mtx);
        v5C.x = bone->mtx[0][3] - joint.x;
        v5C.y = bone->mtx[1][3] - joint.y;
        v5C.z = bone->mtx[2][3] - joint.z;
        vec_normalize_len(&v5C);
        v14.x = mtxa->mtxA[0][2];
        v14.y = mtxa->mtxA[1][2];
        v14.z = mtxa->mtxA[2][2];
        vec_cross(&v14, &v5C, &v8);
        vec_normalize_len(&v8);
        vec_cross(&v5C, &v8, &v14);
        mtxa->mtxA[0][0] = v5C.x;
        mtxa->mtxA[1][0] = v5C.y;
        mtxa->mtxA[2][0] = v5C.z;
        mtxa->mtxA[0][1] = v8.x;
        mtxa->mtxA[1][1] = v8.y;
        mtxa->mtxA[2][1] = v8.z;
        mtxa->mtxA[0][2] = v14.x;
        mtxa->mtxA[1][2] = v14.y;
        mtxa->mtxA[2][2] = v14.z;
        mathutil_mtxA_set_translate(&joint);
        mtxa_to_mtx(chain[0]->mtx);
    }
    else
        mtxa_to_mtx(chain[0]->mtx);
    mtxa_translate_xyz(bone->len, 0.0f, 0.0f);
    if (bone->flags & 4)
        mtxa_sq_from_identity();
    mtxa_to_mtx(bone->mtx);
    }
}

void ape_archive_offsets_to_pointers(u32 *archive)
{
    u32 *p = archive;
    u32 count;
    u32 i;

#ifndef __MWERKS__
    {
        void pc_swap_ape_archive(void *archive);
        pc_swap_ape_archive(archive); /* PC port: archive data is big-endian on disc */
    }
#endif
    count = *p++;

    if ((*archive & 0x80000000) == 0)
    {
        for (i = 0; i <= count; i++)
            p[i] += (u32)archive;
        *archive |= 0x80000000;
    }
}

char *fn_1_10DFDC(struct Mta *mta, u32 idx)
{
    char *name = mta->names;

    if (idx >= (mta->count & 0x7FFFFFFF))
        return NULL;
    while (idx != 0)
    {
        name += strlen(name) + 1;
        idx--;
    }
    return name;
}

int g_table_index(struct Mta *mta, char *name)
{
    u32 count = mta->count & 0x7FFFFFFF;
    char *s = mta->names;
    u32 i;

    for (i = 0; i < count; i++)
    {
        if (strcmp(name, s) == 0)
            return i;
        s += strlen(s) + 1;
    }
    return -1;
}

static inline double msl_sqrt(double x)
{
    if (x > 0.0)
    {
        double guess = __frsqrte(x);

        guess = 0.5 * guess * (3.0 - x * (guess * guess));
        guess = 0.5 * guess * (3.0 - x * (guess * guess));
        guess = 0.5 * guess * (3.0 - x * (guess * guess));
        guess = 0.5 * guess * (3.0 - x * (guess * guess));
        return x * guess;
    }
    else if (0.0 == x)
        return 0.0;
    else if (x)
        return lbl_80198AC0;
    return lbl_80198AC4;
}

int fn_1_10E0D8(float r1, float r2, Vec *a, Vec *b, Vec *c, Vec *d, float *out)
{
    double C, dx, dy, dz, rx, ry, rz, A, B, disc;

    *out = 100.0f;
    dx = (double)a->x - c->x;
    dy = (double)a->y - c->y;
    dz = (double)a->z - c->z;
    rx = ((double)b->x - d->x) - dx;
    ry = ((double)b->y - d->y) - dy;
    rz = ((double)b->z - d->z) - dz;
    A = rz * rz + (rx * rx + ry * ry);
    if (1.1920928955078125e-07 > A)
        return 0;
    C = r1 + r2;
    B = dz * rz + (dx * rx + dy * ry);
    C = dz * dz + (dx * dx + dy * dy) - C * C;
    disc = B * B - A * C;
    if (0.0 > disc)
        return 0;
    if (0.0 >= C)
    {
        *out = 0.0f;
        return 1;
    }
    if (!(0.0 >= C + (B + (A + B))))
    {
        if (0.0 <= B)
            return 0;
        if (-B >= A)
            return 0;
    }
    *out = -(B + msl_sqrt(disc)) / A;
    return 1;
}

float fn_1_10E394(double x1, double y1, double x2, double y2, double px, double py)
{
    double d, d2, cross, len, perp, u;

    d = (x1 - px) * (x1 - px) + (y1 - py) * (y1 - py);
    d2 = (x2 - px) * (x2 - px) + (y2 - py) * (y2 - py);
    cross = y1 * (x1 - x2) + (x1 * (y2 - y1) + (px * (y1 - y2) + py * (x2 - x1)));
    len = (y2 - y1) * (y2 - y1) + (x1 - x2) * (x1 - x2);
    perp = cross * cross / len;
    d = (d > d2) ? d2 : d;
    if (d > perp)
    {
        u = -((x1 - px) * (x2 - x1) + (y1 - py) * (y2 - y1)) / len;
        if (u >= 0.0 && u <= 1.0)
            d = perp;
    }
    return msl_sqrt(d);
}

int fn_1_10E5B8(float r, Vec *a, Vec *b, Vec *c, Vec *out, s8 which, float *tOut)
{
    double dx, dz, A, B, C, disc, t2, t1, root, t, x, z;

    dx = (double)b->x - a->x;
    dz = (double)b->z - a->z;
    if (__fabs((float)dx) < 1.1920928955078125e-07 && __fabs((float)dz) < 1.1920928955078125e-07)
        return 0;
    A = dx * dx + dz * dz;
    B = dx * ((double)a->x - c->x) + dz * ((double)a->z - c->z);
    C = (double)c->z * c->z + ((double)c->x * c->x + (((double)a->x * a->x + (double)a->z * a->z) - 2.0 * ((double)c->x * a->x + (double)c->z * a->z))) - (double)r * r;
    disc = B * B - A * C;
    if (disc < 0.0)
        return 0;
    root = msl_sqrt(disc);
    t1 = (-B + root) / A;
    t2 = (-B - root) / A;
    if ((t1 < 0.0 || t1 > 1.0) && (t2 < 0.0 || t2 > 1.0))
    {
        if (which == 0)
            t = t2;
        else if (which == 1)
            t = t1;
        else
            return 0;
    }
    else if (t1 >= 0.0 && t1 <= 1.0 && t2 >= 0.0 && t2 <= 1.0)
    {
        if (t1 * dz * (t1 * dz) + t1 * dx * (t1 * dx) < t2 * dz * (t2 * dz) + t2 * dx * (t2 * dx))
            t = t1;
        else
            t = t2;
    }
    else if (t1 > 0.0 && t1 <= 1.0)
        t = t1;
    else
        t = t2;
    x = a->x + t * dx;
    z = a->z + t * dz;
    if (__fabs(a->x - b->x) > __fabs(a->z - b->z))
        *tOut = (x - a->x) / ((double)b->x - a->x);
    else
        *tOut = (z - a->z) / ((double)b->z - a->z);
    out->y = 0.0f;
    out->x = x;
    out->z = z;
    return 1;
}

int fn_1_10EA30(Vec *p, Vec *a, Vec *b, Vec *out)
{
    float dx, dz, dot, len, t;

    dx = b->x - a->x;
    dz = b->z - a->z;
    if (__fabs(dx) < 1.1920928955078125e-07 && __fabs(dz) < 1.1920928955078125e-07)
        return 0;
    dot = dx * (p->x - a->x) + dz * (p->z - a->z);
    len = dx * dx + dz * dz;
    if (len > 0.0f && (dot < 0.0f || dot > len))
        return 0;
    if (len < 0.0f && (dot > 0.0f || dot < len))
        return 0;
    t = dot / len;
    out->x = a->x + dx * t;
    out->z = a->z + dz * t;
    return 1;
}
