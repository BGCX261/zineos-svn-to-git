/**
 * Copyright (c) 2010, ZineOS development team
 * All rights _memregions.
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

#include <stdio.h>
#include <string.h>
#include "kernel/common.h"
#include "kernel/system.h"
#include "kernel/frame.h"


// the frame stack pointer
//static uint64_t * frame_stack = NULL;

// Memory entries. upto 256 entries. Once loaded this frame will be released
uint64_t g_mem[256][2];     //Start, End (last 12 bits are used for flags)

// the _memregions index
static uint8_t mem_index = 0;

// Manage region. Add as _memregions or as allowed
//static void ManageRange(uint64_t addr, uint64_t length, bool _memregions);

// debug dump
//static void Dump();

// Find free area for the map
//static void * AllocBitmap(size_t size);

/**
 * Setup the frame allocator
 * pass amount of available physical memory
 * pass the address where to allocate the stack
 *
void Frame::Install()
{
    // find highest possible address
    // since entries are ordered and sanitized
    // simply check the last entry :)
    uint64_t max_addr = _mem.usable[free_index - 1][1];

    // the size of the bitmap to handle all the free memory
    size_t page_count = max_addr / SIZE_4KB;
    size_t bitmap_size = ALIGN(page_count / 8, SIZE_4KB);
    void * bitmap_addr = AllocBitmap(bitmap_size);

    // dump
    Dump();

    // mark whole area as used
    memset(bitmap_addr, 0xF, bitmap_size);

    // last reserve entry
    size_t rsrv = 0;

    // iterate through usable regions
    for (size_t i = 0; i < free_index; i++)
    {
        auto & usable = _mem.usable[i];

        uint64_t start = usable[0];
        uint64_t end = usable[1];
        size_t pages = (end - start) / SIZE_4KB;

        while (start < end)
        {
            if (start >= _mem.reserved[rsrv][0])
            {
                start = _mem.reserved[rsrv][1];
                rsrv += 1;
                continue;
            }
        }
    }
}


/**
 * Allocate bitmap area
 *
void * AllocBitmap(size_t size)
{

    // find memory location where to store the bitmap
    for (size_t i = 0; i < free_index; i++)
    {
        auto & usable = _mem.usable[i];
        // below 1MB
        if (usable[1] <= SIZE_1MB) continue;
        // above 4GB
        if (usable[0] >= SIZE_4GB) continue;

        // find space at the back
        uint64_t start = usable[0];
        uint64_t target = usable[1] - size;

        // above 4GB ?
        if (target > SIZE_4GB) target = SIZE_4GB;

        // below 1MB ?
        if (start < SIZE_1MB) start = SIZE_1MB;

        // conflict flag
        bool conflict = false;

        // search for available slot
        while (target > start)
        {
            // reset conflict
            bool conflict = false;

            // iterate through reserved areas
            for (size_t x = 0; x < resv_index; x++)
            {
                auto & reserved = _mem.reserved[x];

                // beyond
                if (reserved[0] >= (target + size)) continue;

                // before
                if (reserved[1] <= target) continue;

                // conflict
                conflict = true;

                // set new target that is safe from this one
                target = reserved[0] - size;

                // no point in continuing
                break;
            }

            // no conflict
            if (!conflict) break;
        }

        // success in finding?
        if (!conflict)
        {
            Frame::ReserveRange(target, size);
            return (void *)target;
            break;
        }
    }

    return 0;
}


/**
 * Allocate x pages of memory
 */
void * Frame::Alloc(uint64_t max_addr, uint64_t size, uint64_t type)
{
    size_t flags, start, end;
    size = ALIGN(size, 0x1000);             //Rounds up

    for (int i = mem_index - 1; i != -1; --i)
    {
        flags = g_mem[i][0] & FRAME_FLAGS;
        //First verify it's free space..
        if ((flags & FRAME_FREE) == FRAME_FREE)
        {
            start = FRAME_CLEARFLAGS(g_mem[i][0]);
            end = FRAME_CLEARFLAGS(g_mem[i][1]);
             //Enough room under our max_address, and fits within this block?
            if ((start + size <= max_addr) && (start + size <= end))
            {
                ReserveRange(start, size, type);
                return (void*)start;
            }
        }
    }
    return 0;   //Nothing found!
}


/**
 * Release some memory
 */
void Frame::Free(void * frame)
{
    uint64_t location;
    for (int i = 0; i != mem_index; ++i)
    {
        location = FRAME_CLEARFLAGS(g_mem[i][0]);
        if (location == (uint64_t)frame) //This our sucker?
        {
            //Set it as free!
            g_mem[i][0] = location | FRAME_FREE;
            return;
        }
    }
    //ToDo:  Maybe some error checking saying it was invalid release?
}


/**
 * Add range of available (free) memory.
 */
void Frame::AddRange(uint64_t addr, uint64_t length)
{
    // align to page boundries
    uint64_t start = ALIGN(addr, 0x1000);
    uint64_t end =  ALIGN_FLOOR(addr + length, 0x1000);
    if (mem_index < 256)
    {
         g_mem[mem_index][0] = start | FRAME_FREE;
        g_mem[mem_index][1] = end;
        ++mem_index;
    }
    else
    { //We ran out of memory ranges?
        printf("Not enough space to add 0x%x%x\n", MAKE_64_32x2(addr));
    }
}


/**
 * Reserve range of memory, type is either FRAME_USED or FRAME_AVAILABLE
 */
void Frame::ReserveRange(uint64_t addr, uint64_t length, uint64_t type)
{
    // align to page boundries
    uint64_t start = ALIGN_FLOOR(addr, 0x1000);
    uint64_t end =  ALIGN(addr + length, 0x1000);

    if (mem_index == 255)    //Always be safe and say we need 2 spaces free
    {
        printf("Frame::ReserveRange failed, not enough entry space\n");
        return;
    }
/*
    printf("Start ");
    PRINT_64(start);
    printf(" - ");
    PRINT_64(end);
    printf("\n");
*/
    //This should fall within one of our ranges of free memory...
    for (size_t i=0; i != mem_index; ++i)
    {
        //First only search free ranges...
        if ((g_mem[i][0] & FRAME_FREE) == FRAME_FREE)
        {
            uint64_t mem_start = FRAME_CLEARFLAGS(g_mem[i][0]);
            uint64_t mem_end = FRAME_CLEARFLAGS(g_mem[i][1]);
            if (start >= mem_start && end <= mem_end)
            {
                if (start == mem_start && end == mem_end)
                {
                    //Yay, it lines up perfectly?
                    g_mem[i][0] = start | type;
                    g_mem[i][1] = end;
                }
                else
                {
                    //Add the entry..
                    g_mem[mem_index][0] = start | type;
                    g_mem[mem_index][1] = end;
                    ++mem_index;
                    if (start == mem_start)
                    {
                        //Did it start at the same spot as us?
                        g_mem[i][0] = end | FRAME_FREE;
                    }
                    else if (end == mem_end)
                    {
                        //Did it end at the same spot as we end?
                        g_mem[i][1] = start;
                    }
                    else    //It was partially in between :P
                    {
                        //Add a new free entry to encompass the rest of what
                        //was free
                        g_mem[mem_index][0] = end | FRAME_FREE;
                        g_mem[mem_index][1] = g_mem[i][1];
                        ++mem_index;

                        //Now set us to whatever we had left of the beginning
                        g_mem[i][1] = start;
                    }
                }
                //We found it
                return;
            }
        }
    }
    //ToDo: Error message if not found within range?
}


/**
 * Add memory region. Either _memregions or allowed
 *
static void ManageRange(uint64_t start, uint64_t end, bool reserved)
{
    // data references
    auto & the_index = reserved ? resv_index : free_index;
    auto & region = reserved ? _mem.reserved : _mem.usable;

    // do intelligent insert. Try to eliminate overlapping
    // and continuous entries. Will make setup much easier
    size_t target = the_index;
    if (the_index > 0)
    {
        for (size_t i = 0; i < the_index; i++)
        {
            auto & entry = region[i];

            // start address matches
            if (start == entry[0])
            {
                if (end > entry[1])
                {
                    goto expand;
                }
                return;
            }
            // before
            else if (start < entry[0])
            {
                // at least partial overlap
                if (end >= entry[0])
                {
                    entry[0] = start;

                    // encloses wholly
                    if (end > entry[1])
                    {
                        goto expand;
                    }
                    return;
                }

                // shift
                target = i;
                break;
            }
            // after
            else if (start <= entry[1])
            {
                // expand ?
                if (end > entry[1])
                {
                    goto expand;
                }
                return;
            }

            // just continue
            continue;

            // expand
            expand:
            entry[1] = end;
            target = i;

            while (i < (the_index - 1) && end >= region[i + 1][0]) i++;
            if (i != target)
            {
                size_t diff = i - target;
                // expand current end ?
                if (end < region[i][1])
                {
                    entry[1] = region[i][1];
                }
                // move ?
                if ((i + 1) < the_index)
                {
                    memmove(
                        (void *)&region[target + 1],
                        (void *)&region[i + 1],
                        (sizeof(region[0]) * (the_index - i))
                    );
                }
                the_index -= diff;
            }
            return;
        }
    }

    // shift data ?
    if (target != the_index)
    {
        memmove(
            (void *)&region[target + 1],
            (void *)&region[target],
            sizeof(region[0]) * ((the_index - target))
        );
    }

    // increment
    the_index++;

    // set
    region[target][0] = start;
    region[target][1] = end;
}


/**
 * Display information about memory
 */
void Frame::Display(uint64_t type)
{
    printf(" #  Start                End\n");
    for (size_t i = 0; i < mem_index; i++)
    {
        if ((g_mem[i][0] & FRAME_FLAGS) & type)
        {
            printf
                ("%*d  0x%x%x - 0x%x%x",
                    2, i + 1,
                    MAKE_64_32x2(g_mem[i][0]), MAKE_64_32x2(g_mem[i][1])
                );
            printf("\n");
        }
    }
}


/**
 * Used to make a copy of our map for the actual kernel
 */
void MakeKernelMap(void)
{
    void *k_map = Frame::Alloc(FRAME_SUB_4GB, 4096, FRAME_USED);

    memcpy(k_map, g_mem, 4096);

    g_system_params.mem_table =PTR_TO_U64(k_map);
}
