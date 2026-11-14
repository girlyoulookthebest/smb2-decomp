/* Sample minigame module: a counter on the debug text layer. */
#include "types.h"
#include "mode.h"

void g_very_similar_to_puts(char *s);
void printf(char *fmt, ...);
void OSPanic(char *file, int line, char *fmt, ...);
void *OSGetStackPointer(void);
void debug_textdraw_set_pos(int x, int y);
void debug_textdraw_vsprintf_buf_1(char *fmt, ...);
void g_set_minigame_specific_funcs(void (*tick)(void), void (*draw)(void), void (*c)(void), void (*d)(void));

void _prolog(void);
void _epilog(void);
void _unresolved(void);
void fn_7_128(void);
void fn_7_148(void);

u32 lbl_7_data_0[] = {
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

static s32 lbl_7_bss_0;

void _prolog(void)
{
    g_set_minigame_specific_funcs(fn_7_128, fn_7_148, NULL, NULL);
    sub_mode_request = 0xB2;
    g_very_similar_to_puts("RELOCATION SAMPLE LOADED\n");
}

void _epilog(void)
{
    g_very_similar_to_puts("RELOCATION SAMPLE PURGED\n");
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
    OSPanic("rel_sample.c", 77, "\n");
}

void fn_7_128(void)
{
    lbl_7_bss_0++;
}

void fn_7_148(void)
{
    debug_textdraw_set_pos(0, 10);
    debug_textdraw_vsprintf_buf_1("SAMPLE: %d\n", lbl_7_bss_0);
}
