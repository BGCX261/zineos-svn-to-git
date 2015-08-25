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
#pragma once

#include <stddef.h>
#include <stdint.h>

#define FRAME_FLAGS         4095
#define FRAME_CLEARFLAGS(v) ((v) & (~4095))
//An unset frame
#define FRAME_UNSET     0x00
//Free for use
#define FRAME_FREE      0x01
//In use by kernel
#define FRAME_USED      0x02
//Available after loader is complete
#define FRAME_AVAILABLE 0x04
//Just incase ;)
#define FRAME_UNKNOWN   0x08

//Defines allocation locations
#define FRAME_SUB_1MB   1024*1024
#define FRAME_SUB_16MB  1024*1024*16
#define FRAME_SUB_4GB   0x100000000
#define FRAME_NO_LIMIT  0xffffffffffffffff

namespace Frame
{
    /**
     * Setup the frame allocator
     * pass amount of available physical memory
     */
    void Install();

    /**
     * Used to make a copy of our map for the actual kernel
     */
    void MakeKernelMap(void);

    /**
     * Allocate SUB max_addr, size bytes, and USED or AVAILABLE
     */
    void *Alloc(uint64_t max_addr, uint64_t size, uint64_t type);


    /**
     * Release some memory
     */
    void Free(void *frame);


    /**
     * Reserve range of memory.
     */
    void ReserveRange(uint64_t addr, uint64_t length, uint64_t type);


    /**
     * Add range of available (free) memory.
     */
    void AddRange(uint64_t addr, uint64_t length);


    /**
     * Display a a specific type of memory
     */
    void Display(uint64_t type);
}
