/*
 * stdio.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Standard I/O
 *
 */

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void * FILE;
#define stdout (FILE)0
#define stderr (FILE)1

/*
 * Limited printd. But does the hob. From linux 1.0
 */
int printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/*
 * print a char to screen
 */
int putchar (int character);



#ifdef __cplusplus
};
#endif

#endif // STDIO_H_INCLUDED
