#ifndef PC_HFR_H
#define PC_HFR_H

/* PC port: hints for the high frame rate renderer (pc_port docs/hfr.md).
 * Draws issued between HFR_TAG(kind, id) and HFR_UNTAG() carry that
 * identity, so the renderer pairs them with the same object's draws of the
 * previous tick even when objects appear, disappear or change order.
 * HFR_CUT() marks a camera discontinuity (nothing is blended across it),
 * HFR_VIEW() reports a camera's view matrix (cut detection).  All of these
 * are empty in the GameCube build. */
#ifndef __MWERKS__
void pc_hfr_tag_push(unsigned kind, unsigned id);
void pc_hfr_tag_pop(void);
void pc_hfr_cut(int reason);
void pc_hfr_view(int camera, const float (*mtx)[4]);
void pc_hfr_node_alloc(void *node);
void pc_hfr_node_begin(void *node, void *drawFunc);
void pc_hfr_node_end(void);
#define HFR_TAG(kind, id) pc_hfr_tag_push((kind), (unsigned)(id))
#define HFR_UNTAG() pc_hfr_tag_pop()
#define HFR_CUT(reason) pc_hfr_cut(reason)
#define HFR_VIEW(cam, mtx) pc_hfr_view((cam), (const float (*)[4])(mtx))
#define HFR_NODE_ALLOC(node) pc_hfr_node_alloc(node)
#define HFR_NODE_BEGIN(node, func) pc_hfr_node_begin((node), (void *)(func))
#define HFR_NODE_END() pc_hfr_node_end()
#else
#define HFR_TAG(kind, id)
#define HFR_UNTAG()
#define HFR_CUT(reason)
#define HFR_VIEW(cam, mtx)
#define HFR_NODE_ALLOC(node)
#define HFR_NODE_BEGIN(node, func)
#define HFR_NODE_END()
#endif

/* tag kinds */
#define HFR_K_BALL       1
#define HFR_K_ITEM       2
#define HFR_K_STOBJ      3
#define HFR_K_EFFECT     4
#define HFR_K_STAGE_IG   5
#define HFR_K_STAGE_INST 6
#define HFR_K_BG         7
#define HFR_K_SPRITE     8
#define HFR_K_MINI       9   /* party-game objects: id = object index */

/* cut reasons */
#define HFR_CUT_CAMERA_RESET 1
#define HFR_CUT_MODE         2
#define HFR_CUT_SCENE        3

#endif
