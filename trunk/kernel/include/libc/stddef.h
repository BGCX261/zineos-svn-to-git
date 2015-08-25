/*
 * stddef.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Dtandard definations
 *
 */

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED

#ifdef __cplusplus
    #define NULL 0
    extern "C" {
#else
    #define NULL (void *)0
#endif

typedef unsigned long size_t;

#ifdef __cplusplus
    };
#endif

#endif // STDDEF_H_INCLUDED
