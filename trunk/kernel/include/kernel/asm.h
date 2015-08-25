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

#ifndef SYS_ASM_H_INCLUDED
#define SYS_ASM_H_INCLUDED

#include <kernel/common.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Enable interrups..
 */
INLINE_FUNCTION (void sti ())
{
    asm volatile ("sti");
}


/**
 * Disable interrups
 */
INLINE_FUNCTION (void cli ())
{
    asm volatile ("cli");
}


/**
 * read from port
 */
INLINE_FUNCTION (unsigned char inportb (unsigned short _port))
{
    unsigned char rv;
    asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}


/**
 * Write to the port
 */
INLINE_FUNCTION (void outportb (unsigned short _port, unsigned char _data))
{
    asm volatile ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


/**
 * Halt the system
 */
INLINE_FUNCTION (void halt ())
{
    cli();
    for (;;);
}



#ifdef __cplusplus
};
#endif

#endif // SYS_ASM_H_INCLUDED
