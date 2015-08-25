/* multiboot.h - Multiboot header file. */

/* Copyright (C) 1999,2003,2007,2008,2009  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "kernel/common.h"


namespace MBoot
{

    /**
     * constants for the multiboot
     */
    enum Options
    {
        HEADER                  = 1,
        // search bytes for multiboot
        SEARCH                  = 8192,
        // header magic field
        HEADER_MAGIC            = 0x1BADB002,
        // bootloader magic field (in eax)
        BOOTLOADER_MAGIC        = 0x2BADB002,
        // unsupported bits
        UNSUPPORTED             = 0x0000fffc,

        // Alignment of multiboot modules
        MOD_ALIGN               = 0x00001000,
        // Alignment of the multiboot info structure
        INFO_ALIGN              = 0x00000004,
        // Align all boot modules on i386 page (4KB) boundaries
        PAGE_ALIGN              = 0x00000001,
        // Must pass memory information to OS
        MEMORY_INFO             = 0x00000002,
        // Must pass video information to OS
        VIDEO_MODE              = 0x00000004,
        // This flag indicates the use of the address fields in the header
        AOUT_KLUDGE             = 0x00010000,

        // is there basic lower/upper memory information?
        INFO_MEMORY             = 0x00000001,
        // is there a boot device set?
        INFO_BOOTDEV            = 0x00000002,
        // is the command-line defined?
        INFO_CMDLINE            = 0x00000004,
        // are there modules to do something with?
        INFO_MODS               = 0x00000008,
        // is there a symbol table loaded?
        INFO_AOUT_SYMS          = 0x00000010,
        // is there an ELF section header table?
        INFO_ELF_SHDR           = 0X00000020,
        // is there a full memory map?
        INFO_MEM_MAP            = 0x00000040,
        // Is there drive info?
        INFO_DRIVE_INFO         = 0x00000080,
        // Is there a config table?
        INFO_CONFIG_TABLE       = 0x00000100,
        // Is there a boot loader name?
        INFO_BOOT_LOADER_NAME   = 0x00000200,
        // Is there a APM table?
        INFO_APM_TABLE          = 0x00000400,
        // Is there video information?
        INFO_VIDEO_INFO         = 0x00000800,

        // memory map available memory
        MEMORY_AVAILABLE        = 1,
        // memory map reserved memory
        MEMORY_RESERVED         = 2
    };


    /**
     * ELF sections
     */
    struct ElfSectionHeaderTable
    {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
    };

    /**
     * ModuleTable
     */
    struct ModuleTable
    {
        // the module start address
        uint32_t mod_start;

        // the module end address
        uint32_t mod_end;

        // Module command line
        uint32_t cmdline;

        // padding to take it to 16 bytes (must be zero)
        uint32_t pad;

        /**
         * Get module start address
         */
        inline void * GetStart() const
        {
            return (void *)mod_start;
        }


        /**
         * Get end address
         */
        inline void * GetEnd() const
        {
            return (void *)mod_end;
        }


        /**
         * Get size in bytes
         */
        inline size_t GetSize() const
        {
            return (mod_end - mod_start);
        }


        /**
         * Command line
         */
        inline const char * GetCmdLine() const
        {
            return (const char *)cmdline;
        }


        /**
         * Get padding
         */
        inline uint32_t GetPadding() const
        {
            return pad;
        }


        /**
         * Debug output
         */
         void Dump() const;
    };


    /**
     * MemoryMapTable
     */
    struct MemoryMapTable
    {

        // struct size (20)
        uint32_t size;

        // start address
        uint64_t addr;

        // length in bytes
        uint64_t len;

        // region type
        uint32_t type;


        /**
         * Get memory map entry size (ignore?)
         */
        inline uint32_t GetSize() const
        {
            return size;
        }


        /**
         * Get map start address
         */
        inline uint64_t GetAddr() const
        {
            return addr;
        }


        /**
         * Get map end address
         */
        inline uint64_t GetEnd() const
        {
            return addr + len;
        }


        /**
         * Get mapped region size in bytes
         */
        inline uint64_t GetLen() const
        {
            return len;
        }


        /**
         * Get mapped region type
         */
        inline uint32_t GetType() const
        {
            return type;
        }


        /**
         * Debug output
         */
        void Dump() const;

    } __attribute__((packed));


    /**
     * Multiboot InfoTable
     */
    struct InfoTable
    {
        // Multiboot info version number
        uint32_t flags;

        // Available memory from BIOS
        uint32_t mem_lower;
        uint32_t mem_upper;

        // "root" partition
        uint32_t boot_device;

        // Kernel command line
        uint32_t cmdline;

        // Boot-Module list
        uint32_t mods_count;
        uint32_t mods_addr;

        // elf
        ElfSectionHeaderTable elf_sec;

        // Memory Mapping buffer
        uint32_t mmap_length;
        uint32_t mmap_addr;

        // Drive Info buffer
        uint32_t drives_length;
        uint32_t drives_addr;

        // ROM configuration table
        uint32_t config_table;

        // Boot Loader Name
        uint32_t boot_loader_name;

        // APM table
        uint32_t apm_table;

        // Video
        uint32_t vbe_control_info;
        uint32_t vbe_mode_info;
        uint16_t vbe_mode;
        uint16_t vbe_interface_seg;
        uint16_t vbe_interface_off;
        uint16_t vbe_interface_len;


        /**
         * Get module count. Return 0 if none
         */
        inline size_t GetModuleCount() const
        {
            if (flags & INFO_MODS && mods_addr != NULL)
            {
                return mods_count;
            }
            return 0;
        }


        /**
         * Get module. wrong index behaviour is undefined
         */
        inline ModuleTable * GetModule(size_t index) const
        {
            size_t cnt = GetModuleCount();
            if (cnt <= index) return NULL;

            auto modules = (ModuleTable *)mods_addr;
            return modules + index;
        }


        /**
         * Find module by command line
         */
        inline ModuleTable * FindModuleByCmdLine(const char * cmdline) const
        {
            size_t cnt = GetModuleCount();
            size_t len = strlen(cmdline);
            for (size_t i = 0; i < cnt; i++)
            {
                auto module = (ModuleTable *)mods_addr + i;
                if (memcmp(cmdline, module->GetCmdLine(), len) == 0)
                {
                    return module;
                }
            }
            return NULL;
        }


        /**
         * Get modules table start address
         */
        inline void * GetModsTableAddr() const
        {
            return (void *)mods_addr;
        }


        /**
         * Get the size of the modules table in bytes
         */
        inline size_t GetModsTableSize() const
        {
            return (mods_count * sizeof(ModuleTable));
        }


        /**
         * Debug output of the modules table
         */
        void DumpModules() const;


        /**
         * Get available memory in bytes
         */
        inline uint64_t GetAvailableMemory() const
        {
            if (flags & INFO_MEMORY)
            {
                return mem_upper * SIZE_1KB + SIZE_1MB;
            }
            return 0;
        }


        /**
         * Get memory region count. 0 if not present
         */
        inline size_t GetMemMapCount() const
        {
            if ( !(flags & INFO_MEM_MAP) || mmap_length == 0 )
            {
                return 0;
            }
            return mmap_length / sizeof(MemoryMapTable);
        }


        /**
         * Get module. wrong index behaviour is undefined
         */
        inline MemoryMapTable * GetMemMap(size_t index) const
        {
            size_t cnt = GetMemMapCount();
            if (cnt <= index) return NULL;

            auto maps = (MemoryMapTable *)mmap_addr;
            return maps + index;
        }


        /**
         * Get memory map table address
         */
        inline void * GetMemoryTableAddr() const
        {
            return (void *)mmap_addr;
        }


        /**
         * Get memory map table size in bytes
         */
        inline size_t GetMemoryTableSize() const
        {
            return mmap_length;
        }


        /**
         * Debug output of the memory map table
         */
        void DumpMemoryMap() const;

    };
}
