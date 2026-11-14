#ifndef MACROS_H
#define MACROS_H

/* CodeWarrior lays out a file's .bss in order of first use. This emits a dummy
 * function referencing `var` (stripped by the linker) to pin it earlier. Remove
 * once the real function that uses it first has been decompiled. */
#define FORCE_BSS_ORDER(var) void *_force_bss_order_##var(void) { return (void *)&var; }

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#define CLAMP(val, min, max) ((val) < (min) ? (min) : (val) > (max) ? (max) : (val))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define FLT_EPSILON 1.1920928955078125e-07f

#endif
