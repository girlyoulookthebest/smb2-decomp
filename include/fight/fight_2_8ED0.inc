/* fight_2_8ED0: 0x8ED0-0x9080 and fight_2's .data from 0x390 (effect func install/restore).
   Written by w2. Belongs at the start of fight_2.c (before fight_2_9080.inc): fn_12_8ED0 reaches
   these statics through a .data pool whose base is the 0x170 table, so its TU's .data starts at
   0x390 (splits.txt has it in fight_1.c). */
/* effect funcs (defined below in fight_2) */
void fn_12_9080(struct Effect *effect);
void fn_12_9144(struct Effect *effect);
void fn_12_9B14(struct Effect *effect);
void fn_12_9DEC(struct Effect *effect);
void fn_12_9DF0(struct Effect *effect);
void fn_12_9F08(struct Effect *effect);
void fn_12_A4C8(struct Effect *effect);
void fn_12_A5AC(struct Effect *effect);
void fn_12_A5B0(struct Effect *effect);
void fn_12_A6A8(struct Effect *effect);
void fn_12_AA7C(struct Effect *effect);
void fn_12_ABEC(struct Effect *effect);
void fn_12_ABF0(struct Effect *effect);
void fn_12_ABF4(struct Effect *effect);
void fn_12_ABF8(struct Effect *effect);
void fn_12_AD4C(struct Effect *effect);
void fn_12_AD50(struct Effect *effect);
void fn_12_ADE0(struct Effect *effect);
void fn_12_AF34(struct Effect *effect);
void fn_12_B080(struct Effect *effect);
void fn_12_B084(struct Effect *effect);
void fn_12_B1B4(struct Effect *effect);
void fn_12_B2FC(struct Effect *effect);
void fn_12_B4B4(struct Effect *effect);
void fn_12_B4B8(struct Effect *effect);
void fn_12_B668(struct Effect *effect);
void fn_12_B7B0(struct Effect *effect);
void fn_12_B87C(struct Effect *effect);
void fn_12_C974(struct Effect *effect);
void fn_12_C9B8(struct Effect *effect);
void fn_12_C9FC(struct Effect *effect);
void fn_12_CA40(struct Effect *effect);
void fn_12_B880(struct Effect *effect);
void fn_12_BAC8(struct Effect *effect);
void fn_12_C66C(struct Effect *effect);
void fn_12_C970(struct Effect *effect);
void fn_12_CA84(struct Effect *effect);
void fn_12_CBA8(struct Effect *effect);
void fn_12_CF74(struct Effect *effect);
void fn_12_D11C(struct Effect *effect);
void fn_12_D120(struct Effect *effect);
void fn_12_D28C(struct Effect *effect);
void fn_12_D9C4(struct Effect *effect);
void fn_12_DBA0(struct Effect *effect);
void fn_12_DBA4(struct Effect *effect);
void fn_12_DC34(struct Effect *effect);
void fn_12_DE7C(struct Effect *effect);
void fn_12_DF38(struct Effect *effect);
void fn_12_DF3C(struct Effect *effect);
void fn_12_E12C(struct Effect *effect);
void fn_12_E298(struct Effect *effect);
void fn_12_E3BC(struct Effect *effect);
void fn_12_E3C0(struct Effect *effect);
void fn_12_E458(struct Effect *effect);
void fn_12_EA70(struct Effect *effect);
void fn_12_EB34(struct Effect *effect);
void fn_12_EB38(struct Effect *effect);
void fn_12_EBC8(struct Effect *effect);
void fn_12_EF50(struct Effect *effect);
void fn_12_F12C(struct Effect *effect);

/* .data: the unreferenced table that starts the .data of several TUs */
static u8 lbl_12_data_390[0x170] = {
    0xFE, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0xFE, 0x0D,
    0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x10, 0x00, 0x4C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x38, 0x00, 0xB4, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x18, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00,
};

/* effect funcs (fight_2.c), one per fight effect type */
static struct EffectFuncs lbl_12_data_500 = { fn_12_9080, fn_12_9144, fn_12_9B14, fn_12_9DEC };  /* type 0x29 */
static struct EffectFuncs lbl_12_data_510 = { fn_12_9DF0, fn_12_9F08, fn_12_A4C8, fn_12_A5AC };  /* type 0x2A */
static struct EffectFuncs lbl_12_data_520 = { fn_12_A5B0, fn_12_A6A8, fn_12_AA7C, fn_12_ABEC };  /* type 0x2B */
static struct EffectFuncs lbl_12_data_530 = { fn_12_ABF0, fn_12_ABF4, fn_12_ABF8, fn_12_AD4C };  /* type 0x2C */
static struct EffectFuncs lbl_12_data_540 = { fn_12_AD50, fn_12_ADE0, fn_12_AF34, fn_12_B080 };  /* type 0x2D */
static struct EffectFuncs lbl_12_data_550 = { fn_12_B084, fn_12_B1B4, fn_12_B2FC, fn_12_B4B4 };  /* type 0x2E */
static struct EffectFuncs lbl_12_data_560 = { fn_12_B4B8, fn_12_B668, fn_12_B7B0, fn_12_B87C };  /* type 0x2F */
static struct EffectFuncs lbl_12_data_570 = { fn_12_C974, fn_12_C9B8, fn_12_C9FC, fn_12_CA40 };  /* type 0x27 */
static struct EffectFuncs lbl_12_data_580 = { fn_12_B880, fn_12_BAC8, fn_12_C66C, fn_12_C970 };  /* type 0x28 */
static struct EffectFuncs lbl_12_data_590 = { fn_12_CA84, fn_12_CBA8, fn_12_CF74, fn_12_D11C };  /* type 0x30 */
static struct EffectFuncs lbl_12_data_5A0 = { fn_12_D120, fn_12_D28C, fn_12_D9C4, fn_12_DBA0 };  /* type 0x31 */
static struct EffectFuncs lbl_12_data_5B0 = { fn_12_DBA4, fn_12_DC34, fn_12_DE7C, fn_12_DF38 };  /* type 0x32 */
static struct EffectFuncs lbl_12_data_5C0 = { fn_12_DF3C, fn_12_E12C, fn_12_E298, fn_12_E3BC };  /* type 0x33 */
static struct EffectFuncs lbl_12_data_5D0 = { fn_12_E3C0, fn_12_E458, fn_12_EA70, fn_12_EB34 };  /* type 0x34 */
static struct EffectFuncs lbl_12_data_5E0 = { fn_12_EB38, fn_12_EBC8, fn_12_EF50, fn_12_F12C };  /* type 0x35 */

/* install the fight effect funcs */
void fn_12_8ED0(void)
{
    overwrite_effect_funcs(0x29, &lbl_12_data_500);
    overwrite_effect_funcs(0x2A, &lbl_12_data_510);
    overwrite_effect_funcs(0x2B, &lbl_12_data_520);
    overwrite_effect_funcs(0x2C, &lbl_12_data_530);
    overwrite_effect_funcs(0x2D, &lbl_12_data_540);
    overwrite_effect_funcs(0x2E, &lbl_12_data_550);
    overwrite_effect_funcs(0x2F, &lbl_12_data_560);
    overwrite_effect_funcs(0x27, &lbl_12_data_570);
    overwrite_effect_funcs(0x28, &lbl_12_data_580);
    overwrite_effect_funcs(0x30, &lbl_12_data_590);
    overwrite_effect_funcs(0x31, &lbl_12_data_5A0);
    overwrite_effect_funcs(0x32, &lbl_12_data_5B0);
    overwrite_effect_funcs(0x33, &lbl_12_data_5C0);
    overwrite_effect_funcs(0x34, &lbl_12_data_5D0);
    overwrite_effect_funcs(0x35, &lbl_12_data_5E0);
}

/* restore the default effect funcs */
void fn_12_8FB0(void)
{
    overwrite_effect_funcs(0x29, NULL);
    overwrite_effect_funcs(0x2A, NULL);
    overwrite_effect_funcs(0x2B, NULL);
    overwrite_effect_funcs(0x2C, NULL);
    overwrite_effect_funcs(0x2D, NULL);
    overwrite_effect_funcs(0x2E, NULL);
    overwrite_effect_funcs(0x2F, NULL);
    overwrite_effect_funcs(0x27, NULL);
    overwrite_effect_funcs(0x28, NULL);
    overwrite_effect_funcs(0x30, NULL);
    overwrite_effect_funcs(0x31, NULL);
    overwrite_effect_funcs(0x32, NULL);
    overwrite_effect_funcs(0x33, NULL);
    overwrite_effect_funcs(0x34, NULL);
    overwrite_effect_funcs(0x35, NULL);
}
