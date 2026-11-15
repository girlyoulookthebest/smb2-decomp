/* Monkey Golf 2: ground pattern projected around the ball while aiming (.text 0x383A0-0x38B04). */
#include "golf.h"

void GXLoadTexMtxImm(Mtx m, int id, int type);
void GXLoadTexObj_cached(GXTexObj *obj, int map);
void GXSetNumChans_cached(u8 n);
void GXSetNumIndStages_cached(u8 n);
void GXSetNumTevStages_cached(u8 n);
void GXSetNumTexGens_cached(u8 n);
void GXSetTevAlphaIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevAlphaOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevColorIn_cached(int stage, int a, int b, int c, int d);
void GXSetTevColorOp_cached(int stage, int op, int bias, int scale, u8 clamp, int outReg);
void GXSetTevSwapMode_cached(int stage, int rasSel, int texSel);
void GXSetTexCoordGen2_cached(int dst, int func, int src, int mtx, int normalize, int postMtx);
void g_GXSetTevIndirect_zero_if_different(int stage);
void g_GXSetTevOrder_wrapper(int stage, int coord, int map, int color);
void opti_GXSetChanCtrl(int chan, u8 enable, int ambSrc, int matSrc, u32 lightMask, int diffFn, int attnFn);
void *fn_1_10A69C(void *func);

struct GolfGroundTex
{
    /*0x00*/ struct GolfGroundTexParams params;
    /*0x20*/ GXTexObj *tex1;
    /*0x24*/ GXTexObj *tex2;
    /*0x28*/ Mtx texMtx1;  /* world -> scrolling pattern */
    /*0x58*/ Mtx texMtx2;  /* world -> height fade */
};

struct GolfGroundTex lbl_11_bss_2B48;

void fn_11_387D0(void);
int fn_11_38AE0(void);

/* Sets up the two texture matrices of the ground pattern drawn around the ball. */
int fn_11_383A0(struct GolfGroundTexParams *params)
{
    Vec offset = {1.0f, 0.0f, 0.0f};

    lbl_11_bss_2B48.params = *params;
    lbl_11_bss_2B48.tex1 = lbl_11_bss_2B48.params.model1->texObjs;
    lbl_11_bss_2B48.tex2 = lbl_11_bss_2B48.params.model2->texObjs;
    mtxa_push();
    offset.x *= 8.0f;
    mtxa_from_identity();
    mtxa_rotate_y(math_atan2(balls[modeCtrl.currPlayer].pos.x - lbl_11_bss_38.x,
                             balls[modeCtrl.currPlayer].pos.z - lbl_11_bss_38.z) + 0x4000);
    mtxa_tf_point(&offset, &offset);
    offset.x = balls[modeCtrl.currPlayer].pos.x + offset.x * (g_font_char_var_3 % 360) / 360.0f;
    offset.y = balls[modeCtrl.currPlayer].pos.y + offset.y * (g_font_char_var_3 % 360) / 360.0f;
    offset.z = balls[modeCtrl.currPlayer].pos.z + offset.z * (g_font_char_var_3 % 360) / 360.0f;
    mtxa_from_identity();
    mtxa_scale_xyz(0.125f, 0.125f, 0.0f);
    mtxa_rotate_x(0x4000);
    mtxa_rotate_y(-math_atan2(balls[modeCtrl.currPlayer].pos.x - lbl_11_bss_38.x,
                              balls[modeCtrl.currPlayer].pos.z - lbl_11_bss_38.z));
    mtxa_translate_neg(&offset);
    mtxa_to_mtx(lbl_11_bss_2B48.texMtx1);
    mtxa_from_translate_xyz(0.5f, 0.5f, 0.0f);
    mtxa_scale_xyz(0.0f, -0.5f / lbl_11_bss_2B48.params.height, 0.0f);
    mtxa_translate_neg(&lbl_11_bss_2B48.params.pos);
    mtxa_to_mtx(lbl_11_bss_2B48.texMtx2);
    mtxa_pop();
    return 1;
}

/* Runs drawFunc with fn_11_38AE0 as the model material callback, then restores the old one. */
void fn_11_38774(void (*drawFunc)(void))
{
    void *prev = fn_1_10A69C(fn_11_38AE0);

    if (drawFunc != NULL)
        drawFunc();
    fn_1_10A69C(prev);
}

/* Material setup: two projected texture stages modulating the lit vertex color. */
void fn_11_387D0(void)
{
    int stage;
    int coord;
    int texMtx;
    Mtx viewInv;
    Mtx mtx1;
    Mtx mtx2;

    GXSetNumChans_cached(1);
    opti_GXSetChanCtrl(2, 0, 1, 1, 0, 0, 2);
    opti_GXSetChanCtrl(0, 1, 0, 1, 0, 2, 2);
    mtxa_push();
    mtxa_from_mtx(cameras[modeCtrl.currPlayer].unk14C);
    mtxa_rigid_invert();
    mtxa_to_mtx(viewInv);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(viewInv);
    mtxa_push();
    mtxa_mult_left(lbl_11_bss_2B48.texMtx1);
    mtxa_to_mtx(mtx1);
    mtxa_pop();
    mtxa_push();
    mtxa_mult_left(lbl_11_bss_2B48.texMtx2);
    mtxa_to_mtx(mtx2);
    mtxa_pop();
    mtxa_pop();
    GXSetBlendMode_cached(1, 4, 5, 0);
    GXSetZMode_cached(1, 2, 1);
    stage = 0;
    coord = 0;
    texMtx = 0x1E;

    GXLoadTexMtxImm(mtx1, texMtx, 1);
    GXSetTexCoordGen2_cached(coord, 1, 0, texMtx, 0, 0x7D);
    GXLoadTexObj_cached(lbl_11_bss_2B48.tex1, 0);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevOrder_wrapper(stage, coord, 0, 4);
    GXSetTevColorIn_cached(stage, 0xF, 9, 8, 0xF);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 4, 5, 7);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;
    texMtx += 3;

    GXLoadTexMtxImm(mtx2, texMtx, 1);
    GXSetTexCoordGen2_cached(coord, 1, 0, texMtx, 0, 0x7D);
    GXLoadTexObj_cached(lbl_11_bss_2B48.tex2, 1);
    GXSetTevSwapMode_cached(stage, 0, 0);
    g_GXSetTevIndirect_zero_if_different(stage);
    g_GXSetTevOrder_wrapper(stage, coord, 1, 0xFF);
    GXSetTevColorIn_cached(stage, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(stage, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(stage, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(stage, 0, 0, 0, 1, 0);
    stage++;
    coord++;

    GXSetNumIndStages_cached(0);
    GXSetNumChans_cached(1);
    GXSetNumTevStages_cached(stage);
    GXSetNumTexGens_cached(coord);
}

/* Model material callback. */
int fn_11_38AE0(void)
{
    fn_11_387D0();
    return 1;
}

/* Stands in for a function the linker stripped from this file; only its 1.0f literal
   survives at the end of the rodata pool. */
void fn_11_golf6_stripped(float *out)
{
    *out = 1.0f;
}
