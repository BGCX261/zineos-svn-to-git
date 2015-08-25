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

#ifndef VIRTUAL_H_INCLUDED
#define VIRTUAL_H_INCLUDED

#include <stdint.h>

/**
 * Virtual Address
 * * Bit(s)     Description
 * * 0-11       Physical Page Offset
 * * 12-20      Page Table Offset (PML1)
 * * 21-29      Page Directory Offset (PMl2)
 * * 30-38      Page Directory Pointer Offset (PML3)
 * * 39-47      Page Map Level 4 offset (PML4)
 * * 48-63      Sign extension of bit 47

 * PML4 Bit(s)  Description
        0       P - Present bit (1=yes, 0=no)
        1       W/R (0=read only, 1=writeable)
        2       U/S (0=super only, 1=user)
        3       PWT (Page Write Through, 0=no, 1=yes)
        4       PCD (Page Cache Disable 0=no, 1=yes)
        5       Accessed (0=no, 1=yes)
        6       Dirty (0=no, 1=yes)
        7       PAT, Page Attribute Table Index
        8       Global page (0=no, 1=yes)
        9-11    Available
        12-39   Base Address
        40-51   Reserved, must be 0
        52-62   Available
        63      NXB (No execute bit, sets page as non-executeable)

 */

#define VIRTUAL_FLAG_PRESENT    (1<<0)      //Page Present flag
#define VIRTUAL_FLAG_READWRITE  (1<<1)      //Read and Write flag
#define VIRTUAL_FLAG_USER       (1<<2)      //Super/User flag
#define VIRTUAL_FLAG_PWT        (1<<3)      //Page Write Through
#define VIRTUAL_FLAG_PCD        (1<<4)      //Page Cache Disable
#define VIRTUAL_FLAG_ACCESSED   (1<<5)      //Accessed flag
#define VIRTUAL_FLAG_DIRTY      (1<<6)      //Dirty flag
#define VIRTUAL_FLAG_GLOBAL     (1<<8)      //Global Page flag
#define VIRTUAL_FLAG_NXB        (1<<64)     //No execute flag

namespace VirtualMemory
{
    /**
     * Used to initialize VirtualMemory manager
     */
    void Install(void);

    /**
     * Maps a physical address to virtual
     */
     void Map(uint64_t phys, uint64_t virt, uint64_t flags);

    /**
     * Map a range of phys -> virt addresses
     */
    void MapRange(uint64_t phys, uint64_t virt, uint64_t size, uint64_t flags);
};

#endif
