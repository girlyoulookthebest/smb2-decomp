#ifndef TYPES_H
#define TYPES_H

/* Fixed-size integer types. Kept free of compiler-specific headers so the
 * same sources build with CodeWarrior (GameCube) and GCC (PC/PSP ports). */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

typedef int BOOL;
#define TRUE 1
#define FALSE 0

/* An integer wide enough to hold a pointer.  The GameCube code keeps
 * pointers in u32 fields and u32 data tables; on the 64-bit PC port those
 * become uptr.  On GC (and the 32-bit PC build) this is exactly u32, so the
 * matching build is unchanged.  See pc_port/docs/x64.md. */
#if defined(__MWERKS__) || !defined(__SIZEOF_POINTER__) || __SIZEOF_POINTER__ == 4
typedef unsigned int uptr;
typedef signed int sptr;
#else
typedef unsigned long long uptr;
typedef signed long long sptr;
#endif

/* DPTR: qualifier for pointer fields inside disc/file images (and other
 * data whose 32-bit layout must be kept).  On the x64 PC build (clang,
 * -fms-extensions) it makes the field a 4-byte, zero-extended pointer, so
 * the struct keeps its GameCube layout and the code using the field is
 * unchanged; every such address must be below 4 GiB (all game memory is).
 * Empty on GC and on 32-bit builds.  Write "T * DPTR name", and
 * "T * DPTR * DPTR name" for a pointer to an array of such pointers. */
#if defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8 && !defined(__MWERKS__)
#if !defined(__clang__)
#error "The 64-bit PC build needs clang (__ptr32 for DPTR fields): use pc_port/cmake/llvm-mingw64.cmake"
#endif
#define DPTR __ptr32 __uptr
#else
#define DPTR
#endif

/* Raw byte buffers that hold arrays of pointer-bearing structs (sized for
 * 32-bit pointers in the decomp): multiply by PC_PTR_SCALE.  A struct grows
 * at most 2x when its pointers go from 4 to 8 bytes.  1 on GC / 32-bit. */
#if defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8
#define PC_PTR_SCALE 2
#else
#define PC_PTR_SCALE 1
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __MWERKS__
/* PC: the GC's `psq_st f, 0(p), 1, 2` (GQR2 = u8 quantized store, scale 0)
 * saturates to 0..255 and truncates toward zero.  A plain float -> u8 cast
 * is undefined out of range and wraps on x86 (300 -> 44). */
static inline u8 pc_psq_st_u8(float v)
{
    return v > 0.0f ? (v < 255.0f ? (u8)v : (u8)255) : (u8)0;
}
#endif

#endif
