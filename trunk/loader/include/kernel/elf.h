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

#ifndef ELF_H_INCLUDED
#define ELF_H_INCLUDED

#include <stdint.h>

typedef uint16_t    Elf64_Half;
typedef uint32_t    Elf64_Word;
typedef	int32_t     Elf64_Sword;
typedef uint64_t    Elf64_Xword;
typedef	int64_t     Elf64_Sxword;
typedef uint64_t    Elf64_Addr;
typedef uint64_t    Elf64_Off;
typedef uint16_t    Elf64_Section;
typedef Elf64_Half  Elf64_Versym;


// The ELF file header.  This appears at the start of every ELF file.
typedef struct
{
    unsigned char e_ident[16];
    Elf64_Half    e_type;
    Elf64_Half    e_machine;
    Elf64_Word    e_version;
    Elf64_Addr    e_entry;
    Elf64_Off     e_phoff;
    Elf64_Off     e_shoff;
    Elf64_Word    e_flags;
    Elf64_Half    e_ehsize;
    Elf64_Half    e_phentsize;
    Elf64_Half    e_phnum;
    Elf64_Half    e_shentsize;
    Elf64_Half    e_shnum;
    Elf64_Half    e_shtrndx;
} Elf64_Ehdr;


#define SHT_PROGBITS    1       // .code/.data section
#define SHT_NOBITS      8       // .bss section
#define ET_EXEC		    2		// Executable file
#define EM_X86_64	    62		// AMD x86-64 architecture

/**
 * Section header
 */
typedef struct
{
	Elf64_Word	sh_name;		// Section name (string tbl index)
	Elf64_Word	sh_type;		// Section type
	Elf64_Xword	sh_flags;		// Section flags
	Elf64_Addr	sh_addr;		// Section virtual addr at execution
	Elf64_Off	sh_offset;		// Section file offset
	Elf64_Xword	sh_size;		// Section size in bytes
	Elf64_Word	sh_link;		// Link to another section
	Elf64_Word	sh_info;		// Additional section information
	Elf64_Xword	sh_addralign;	// Section alignment
	Elf64_Xword	sh_entsize;		// Entry size if section holds table
} Elf64_Shdr;

/**
 * Program segment header.
 */
typedef struct {
    Elf64_Word      p_type;
    Elf64_Word      p_flags;
    Elf64_Off       p_offset;
    Elf64_Addr      p_vaddr;
    Elf64_Addr      p_paddr;
    Elf64_Xword     p_filesz;
    Elf64_Xword     p_memsz;
    Elf64_Xword     p_align;
} Elf64_Phdr;

namespace Elf {


    /**
     * Generic error reporting for elf
     */
    void Error(const char *error_ptr, uint64_t value);


    /**
     * Loads 64-bit elf file from memory
     * Returns the 64-bit entry point
    */
    uint64_t Load(Elf64_Ehdr * elf);
};

#endif
