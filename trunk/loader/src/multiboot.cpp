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

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "kernel/common.h"
#include "kernel/multiboot.h"

using namespace MBoot;


/**
 * Debug output
 */
void ModuleTable::Dump() const
{
    printf("0x%p - 0x%p  ", GetStart(), GetEnd());
    printf("%*d  ", 8, GetSize() / 1024);
    printf("%s\n", GetCmdLine());
}


/**
 * Debug output
 */
void MemoryMapTable::Dump() const
{
    PRINT_64(GetAddr());
    printf(" - ");
    PRINT_64(GetEnd());
    printf("  %d %s  ", GetType(), GetType() == 1 ? "Free    " : "Reserved");
    printf("%*d", 8, (uint32_t)(GetLen() / 1024));
    printf("\n");
}


/**
 * Dump modules table
 */
void InfoTable::DumpModules() const
{
    size_t cnt = GetModuleCount();
    if (cnt == 0)
    {
        printf("No modules loaded\n");
        return;
    }

    printf("Loaded modules:\n");
    printf(" #  Start        End               Kb  Cmd\n");
    for (size_t i = 0; i < cnt; i++)
    {
        auto module = GetModule(i);
        printf("%*d  ", 2, i + 1);
        module->Dump();
    }
}


/**
 * Dump of the memory map table
 */
void InfoTable::DumpMemoryMap() const
{
    size_t cnt = GetMemMapCount();
    if (cnt == 0)
    {
        //printf("No memory map loaded\n");
        return;
    }

    printf("Memory map:\n");
    printf(" #  Start                End                 Status            Kb\n");
    for (size_t i = 0; i < cnt; i++)
    {
        auto map = GetMemMap(i);
        printf("%*d  ", 2, i + 1);
        map->Dump();
    }
}
