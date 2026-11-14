/* sel_ngc REL entry points: module prolog/epilog and the unresolved-call trap. */
#include "sel_ngc.h"

void menu_tick(void);
void menu_draw_background(void);
void g_load_select_apes(void);
void g_initialize_sel_ngc(void);
void g_reinitialize_sel_ngc(void);
void free_some_apes(void);
void fn_3_1EF40(void);

/* Header-provided table every TU in this game emits at the start of its .data. */
u32 lbl_3_data_0[] = {
    0xFE000102, 0x03040506, 0x0708090A, 0x0B0CFE0D, 0x0E0FFF00, 0x00010203,
    0x04050607, 0x08090A0B, 0x0C0D0E0F, 0xFF000000, 0x31000000, 0x00000000,
    0x3800B400, 0x00010000, 0x01000000, 0x00020000, 0x10004C00, 0x00030000,
    0x01000000, 0x00040000, 0x18000000, 0x00090000, 0x40000000, 0x00000000,
    0x01000000, 0x000B0000, 0x18000000, 0x000C0000, 0x18000000, 0x00050000,
    0x40000000, 0x00000000, 0x01000000, 0x00070000, 0x18000000, 0x00080000,
    0x3800B400, 0x000D0000, 0x40000000, 0x00000000, 0x18000000, 0x00140000,
    0x40000000, 0x00000000, 0x01000000, 0x00160000, 0x18000000, 0x00170000,
    0x18000000, 0x00180000, 0x18000000, 0x000F0000, 0x40000000, 0x00000000,
    0x01000000, 0x00110000, 0x18000000, 0x00120000, 0x18000000, 0x00130000,
    0x18000000, 0x00000000, 0x18000000, 0x00010000, 0x18000000, 0x00020000,
    0x18000000, 0x00030000, 0x18000000, 0x00040000, 0x18000000, 0x00050000,
    0x18000000, 0x00060000, 0x18000000, 0x00070000, 0x18000000, 0x00080000,
    0x18000000, 0x00090000, 0x18000000, 0x000A0000, 0x18000000, 0x000B0000,
    0x18000000, 0x000C0000, 0x18000000, 0x000D0000, 0x18000000, 0x000E0000,
    0x18000000, 0x000F0000,
};

void _prolog(void)
{
    g_set_some_sel_ngc_global_func_ptrs(menu_tick, menu_draw_background, NULL, NULL);
    g_load_select_apes();
    SoundGroupLoad(0x17);
    switch (sub_mode)
    {
    case 32:
        break;
    case 31:
        g_initialize_sel_ngc();
        break;
    case 33:
        g_reinitialize_sel_ngc();
        break;
    }
    sub_mode_request = 0x20;
    g_very_similar_to_puts("RELOCATION SEL_NGC LOADED");
}

void _epilog(void)
{
    dest_all_events();
    cameras->mode = 3;
    g_something_calls_sndPopGroup();
    free_some_apes();
    fn_3_1EF40();
    disc_queue_status(3);
    g_very_similar_to_puts("RELOCATION SEL_NGC PURGED");
}

void _unresolved(void)
{
    u32 i;
    u32 *sp;

    g_very_similar_to_puts("\nError: A called an unlinked function.\n");
    g_very_similar_to_puts("Address:      Back Chain    LR Save\n");
    i = 0;
    sp = OSGetStackPointer();
    while (sp != NULL && (u32)sp != 0xFFFFFFFF && i++ < 16)
    {
        printf("0x%08x:   0x%08x    0x%08x\n", sp, sp[0], sp[1]);
        sp = (u32 *)sp[0];
    }
    OSPanic("sel_ngc_rel.c", 106, "\n");
}
