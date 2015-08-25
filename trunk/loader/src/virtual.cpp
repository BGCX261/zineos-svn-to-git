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

#include "kernel/common.h"
#include "kernel/frame.h"
#include "kernel/video.h"
#include "kernel/system.h"
#include "kernel/virtual.h"
#include "string.h"
#include "stdio.h"

uint64_t *pml4_table = 0;  //This will point to a 4kb area, 512 entries

/**
 * Used to initialize VirtualMemory manager
 */
void VirtualMemory::Install(void)
{
    //First we need to grab a page for our PML4
    pml4_table = (uint64_t*)Frame::Alloc(FRAME_NO_LIMIT, 4096, FRAME_USED);

    //1:1 mapping for the bottom 1mb
    for (int i = 0; i != 0x100000; i += 0x1000)
        Map
            (
                i, i, VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
                VIRTUAL_FLAG_GLOBAL
            );

    g_system_params.pml4_table = (uint64_t)pml4_table;
}


/**
 * Map a range of phys -> virt addresses
 */
void VirtualMemory::MapRange
    (
        uint64_t phys, uint64_t virt, uint64_t size, uint64_t flags
    )
{
    phys = ALIGN_FLOOR(phys, 0x1000);
    virt = ALIGN_FLOOR(virt, 0x1000);
    size = ALIGN(size, 0x1000);
    while (size)
    {

//        VirtualMemory::Map(phys, phys, flags);
        VirtualMemory::Map(phys, virt, flags);
        phys += 0x1000;
        virt += 0x1000;
        size -= 0x1000;
    }
}


/**
 * Maps a physical address to virtual
 */
void VirtualMemory::Map(uint64_t phys, uint64_t virt, uint64_t flags)
{
//We need to map a physical address to some virtual address
    size_t pml4_index;    //Index into the PML4
    size_t pml3_index;    //Index of the PDP
    size_t pml2_index;    //Index of the PD
    size_t pml1_index;    //Index of the PT

   uint64_t *pml3_table, *pml2_table, *pml1_table;

//Must by 4k aligned!
    phys = ALIGN_FLOOR(phys, 0x1000);
    virt = ALIGN_FLOOR(virt, 0x1000);

    pml1_index = (virt >> 12) & 0x1ff;    // 511
    pml2_index = (virt >> 21) & 0x1ff;    // 511
    pml3_index = (virt >> 30) & 0x1ff;    // 511
    pml4_index = (virt >> 39) & 0x1ff;    // 511

    //Check if our pml4 points to a pml3 (pdp) yet
    if (pml4_table[pml4_index] == 0)
    {
        void *ptr = Frame::Alloc(FRAME_NO_LIMIT, 4096, FRAME_USED);
        memset(ptr, 0, 4096);       //Zeroize it before use!
        pml4_table[pml4_index] = (size_t)ptr | flags; //Setup a new PDP
    }

    //Bits 12-51 form the base address
    pml3_table = (uint64_t*)(pml4_table[pml4_index] & 0x000FFFFFFFFFF000);

    //Check if our PDP (pml3) hs a PD yet
    if (pml3_table[pml3_index] == 0)
    {
        void *ptr = Frame::Alloc(FRAME_NO_LIMIT, 4096, FRAME_USED);
        memset(ptr, 0, 4096);       //Zeroize it before use!
        pml3_table[pml3_index] = (size_t)ptr | flags; //Setup a new PDP
    }

    //Bits 12-51 form the base address
    pml2_table = (uint64_t*)(pml3_table[pml3_index] & 0x000FFFFFFFFFF000);

    //Check if our PD (pml2) hs a PT yet
    if (pml2_table[pml2_index] == 0)
    {
        void *ptr = Frame::Alloc(FRAME_NO_LIMIT, 4096, FRAME_USED);
        memset(ptr, 0, 4096);       //Zeroize it before use!
        pml2_table[pml2_index] = (size_t)ptr | flags; //Setup a new PT
    }

    //Bits 12-51 form the base address
    pml1_table = (uint64_t*)(pml2_table[pml2_index] & 0x000FFFFFFFFFF000);

//Now we have our PT, so lets set it :)
    pml1_table[pml1_index] = (phys & 0x000FFFFFFFFFF000) | flags;
}
