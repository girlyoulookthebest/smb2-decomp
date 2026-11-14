/* Fragments of the ball that broke open at the end of the game. */
#include "types.h"
#include "ape_internal.h"
#include "avdisp.h"
#include "effect.h"
#include "ending.h"
#include "light.h"
#include "mathutil.h"
#include "nl2ngc.h"

void effect_ending_ballfrag_init(struct Effect *effect);
void effect_ending_ballfrag_tick(struct Effect *effect);
void effect_ending_ballfrag_disp(struct Effect *effect);
void effect_ending_ballfrag_dest(struct Effect *effect);

struct BallfragNode
{
    struct OrdTblNode node;
    int lightGroup;
    struct Effect *effect;
};

struct EffectFuncs lbl_1_data_63D50 = {
    effect_ending_ballfrag_init,
    effect_ending_ballfrag_tick,
    effect_ending_ballfrag_disp,
    effect_ending_ballfrag_dest,
};

void g_set_effect_ending_ballfrag(void)
{
    overwrite_effect_funcs(0x13, &lbl_1_data_63D50);
}

void g_clear_effect_ending_ballfrag(void)
{
    overwrite_effect_funcs(0x13, NULL);
}

void effect_ending_ballfrag_init(struct Effect *effect)
{
    effect->timer = 3600;
    effect->cameras = 0xFFFF;
}

void effect_ending_ballfrag_tick(struct Effect *effect)
{
    Vec v;
    float d;

    if (lbl_1_bss_D3354.flags & 8)
        return;
    mtxa_from_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    mtxa_rigid_inv_tf_vec(&effect->vel, &v);
    d = mathutil_vec_dot_prod(&effect->unk8C, &v);
    if (d < 0.0f)
    {
        d *= -0.3f;
        v.x += d * effect->unk8C.x;
        v.y += d * effect->unk8C.y;
        v.z += d * effect->unk8C.z;
    }
    else
    {
        d *= -0.05f;
        v.x += d * effect->unk8C.x;
        v.y += d * effect->unk8C.y;
        v.z += d * effect->unk8C.z;
    }
    v.x *= 0.99f;
    v.y *= 0.99f;
    v.z *= 0.99f;
    mtxa_tf_vec(&v, &effect->vel);
    effect->vel.y += -0.004899995867f;
    effect->pos.x += effect->vel.x;
    effect->pos.y += effect->vel.y;
    effect->pos.z += effect->vel.z;
    effect->rotX = 0.96f * effect->rotX + 6144.0f * effect->vel.y;
    effect->rotY = 0.96f * effect->rotY + 6144.0f * effect->vel.z;
    effect->rotZ = 0.96f * effect->rotZ + 6144.0f * effect->vel.x;
}

void fn_1_109074(struct BallfragNode *node);

void effect_ending_ballfrag_disp(struct Effect *effect)
{
    struct BallfragNode *node;
    struct OrdTblNode *entry;

    mtxa_from_mtxb();
    entry = ord_tbl_get_entry_for_pos(&effect->pos);
    node = ord_tbl_alloc_node(sizeof(*node));
    if (node != NULL)
    {
        node->node.drawFunc = (void *)fn_1_109074;
        node->lightGroup = get_active_light_group();
        node->effect = effect;
        fn_1_CD418(entry, &node->node);
    }
}

void fn_1_109074(struct BallfragNode *node)
{
    struct Effect *effect;

    load_light_group_cached(node->lightGroup);
    effect = node->effect;
    mtxa_from_mtxb_translate(&effect->pos);
    mtxa_rotate_y(effect->rotY);
    mtxa_rotate_x(effect->rotX);
    mtxa_rotate_z(effect->rotZ);
    load_gx_pos_nrm_mtx(mtxa->mtxA, 0);
}

void effect_ending_ballfrag_dest(struct Effect *effect) {}
