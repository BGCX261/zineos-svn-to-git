/*
 * stdarg.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * GCC var_arg support
 *
 */

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;


#ifdef __cplusplus
};
#endif

#endif // STDARG_H_INCLUDED
