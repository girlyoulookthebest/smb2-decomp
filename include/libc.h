#ifndef LIBC_H
#define LIBC_H

/* CodeWarrior builds don't search system headers (-nosyspath); declare what's used.
 * Other compilers use their own C library. */
#ifdef __MWERKS__
void *memset(void *dst, int c, unsigned long n);
void *memcpy(void *dst, const void *src, unsigned long n);
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
char *strcpy(char *dst, const char *src);
#define abs(x) __abs(x)  /* compiler intrinsic */
#else
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif

/* The game's own memcpy (main.dol). */
void *memcpy_handler(void *dst, const void *src, unsigned long n);

#endif
