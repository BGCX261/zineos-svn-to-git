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
#include "kernel/elf.h"
#include "kernel/frame.h"
#include "kernel/video.h"
#include "kernel/system.h"
#include "kernel/virtual.h"
#include "string.h"
#include "stdio.h"


/**
 * Generic error reporting for elf
 */
void Elf::Error(const char * error_ptr, uint64_t value)
{
    //TODO: Some sort of panic should go here
    printf("Invalid ELF64 %s: 0x%x%x\n", error_ptr, MAKE_64_32x2(value));

    //Lets clear interrupts and halt!
    asm("cli");
    asm("hlt");
}


/**
 * Loads 64-bit elf file from memory
 * Returns the 64-bit entry point
 */
uint64_t Elf::Load(Elf64_Ehdr * elf)
{
    Elf64_Ehdr *elf_header = elf;
    Elf64_Phdr *program_header;
    Elf64_Shdr *section_header;

    uint64_t entry = 0;

    // Verify that we have an Executeable ELF64 File
    if  (   elf_header->e_ident[0] != 0x7f ||
            elf_header->e_ident[1] != 'E' ||
            elf_header->e_ident[2] != 'L' ||
            elf_header->e_ident[3] != 'F')
    {
        Elf::Error("IDENT", *(uint64_t*)elf_header->e_ident);
    }
    if (elf_header->e_type !=  ET_EXEC)
    {
        Elf::Error("Type", elf_header->e_type);
    }
    if (elf_header->e_machine != EM_X86_64)
    {
        Elf::Error("Machine",  elf_header->e_machine);
    }

    program_header = (Elf64_Phdr *)(uint64_t(elf) + elf_header->e_phoff);
    section_header = (Elf64_Shdr *)(uint64_t(elf) + elf_header->e_shoff);

    //Loop through all section headers
    for (size_t i = 0; i != elf_header->e_shnum; ++i)
    {
        if (section_header[i].sh_type == SHT_PROGBITS) //.code or .data section?
        {
            void *ptr;
            //First we allocate enough space for it...
            ptr = Frame::Alloc
                (
                    FRAME_SUB_4GB,
                    section_header[i].sh_size,
                    FRAME_USED
                );
            if (ptr == 0)   //Failed to allocate memory
            {
                return 0;
            }
            //Lets copy it to this physical location
            memcpy(
                ptr,
                (void*)(section_header[i].sh_offset + uint64_t(elf)),
                section_header[i].sh_size
            );
            //Now lets map this section virtual and physically
            VirtualMemory::MapRange
                (
                    (uint64_t)ptr, section_header[i].sh_addr,
                    section_header[i].sh_size,
                    VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
                    VIRTUAL_FLAG_GLOBAL
                );
            VirtualMemory::MapRange
                (
                    (uint64_t)ptr, (uint64_t)ptr,
                    section_header[i].sh_size,
                    VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
                    VIRTUAL_FLAG_GLOBAL
                );
            if
                (
                    (elf_header->e_entry >= section_header[i].sh_addr) &&
                    (elf_header->e_entry <= section_header[i].sh_addr +
                    section_header[i].sh_size)
                )
            {
                entry =
                    (uint64_t)ptr +
                    elf_header->e_entry -
                    section_header[i].sh_addr;
            }


        }
        else if (section_header[i].sh_type == SHT_NOBITS)  // .bss
        {
            void *ptr;
            //First we allocate enough space for it...
            ptr = Frame::Alloc
                (
                    FRAME_SUB_4GB,
                    section_header[i].sh_size,
                    FRAME_USED
                );
            if (ptr == 0)   //Failed to allocate memory
            {
                return 0;
            }
            //This is our BSS, so lets just zeroize it
            memset(ptr, 0, section_header[i].sh_size);
            //Map this section in virtually, and physically
            VirtualMemory::MapRange
                (
                    (uint64_t)ptr, section_header[i].sh_addr,
                    section_header[i].sh_size,
                    VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
                    VIRTUAL_FLAG_GLOBAL
                );
            VirtualMemory::MapRange
                (
                    (uint64_t)ptr, (uint64_t)ptr,
                    section_header[i].sh_size,
                    VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
                    VIRTUAL_FLAG_GLOBAL
                );
        }
    }
    return entry; //This is the 'virtual' location of our entry
}
