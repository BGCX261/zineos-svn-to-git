/**
 * Copyright (c) 2010, ZineOS development team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution. Neither the name of the ZineOS nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SIZE_1KB    1024
#define SIZE_4KB    4096
#define SIZE_1MB    0x100000
#define SIZE_16MB   0x1000000
#define SIZE_4GB    0x100000000

// convert pointers to uint64_t
#define PTR_TO_U64(_p) ((uint64_t)(uintptr_t)(_p))


/*
 * define an inline function
 */
#define INLINE_FUNCTION(_decl) \
    static inline _decl __attribute__((always_inline)); \
    _decl


/*
 * Show a message and HALT if predicate fails
 */
#define ASSERT(_predicate)  \
    if (!(_predicate))      \
    {                       \
        printf (                                            \
            "%s(%d): Assertion failed at %s: %s\n",         \
            __FILE__, __LINE__, __FUNCTION__, #_predicate   \
        );                                                  \
        halt();                                             \
    }


/*
 * PANIC. Show file, line number and function name
 * along with a message. Halt execution after that
 */
#define PANIC(_message)     \
    printf(                                         \
        "PANIC %s(%d) : %s\n%s\n",                  \
        __FILE__, __LINE__, __FUNCTION__, #_message \
    );                                              \
    halt();


/*
 * Display var name and it's value (hex). Add file and line to the message
 */
#define DUMP_INT_VAR(_name) \
    printf(#_name " = 0x%p     %s(%d) : %s\n", (void *)(_name), __FILE__, __LINE__, __FUNCTION__)

/*
 * Display var name and it's value. Add file and line to the message
 */
#define DUMP_INT_VAR_DEC(_name) \
    printf(#_name " = %d     %s(%d) : %s\n", (uint32_t)(_name), __FILE__, __LINE__, __FUNCTION__)


/*
 * Simple way to get the bit
 */
#define BIT(n) (1 << n)

/*
 * simple hackish way to print 64bit addresses
 */
#define PRINT_64BIT_ADDR(_var) \
	printf(#_var " = %p%p\n", (void *)(((uint64_t)_var) >> 32), (void *)_var);

/*
 * Simple hack to display a 64-bit hexidecmial number
 */
#define PRINT_64(_var) \
    printf("0x%p%p", (void *)((_var) >> 32), (void *)(_var));

/*
 * simple hack to convert a single 64-bit number to two 32-bit numbers
 * so you can use as a parameter in printf
 */
#define MAKE_64_32x2(_var)  (uint32_t)((_var)>>32), (uint32_t)(_var)

/*
 * Align the size
 */
#define ALIGN(size, align) (((size) + ((align) - 1)) & (~((align) - 1)))

/*
 * Align the lower address
 */
#define ALIGN_FLOOR(size, align) ((size) & (~((align) - 1)))

// panic and assert use halt()
#include "kernel/asm.h"
#include <stdint.h>


#ifdef __cplusplus
};
#endif

#endif // COMMON_H_INCLUDED
