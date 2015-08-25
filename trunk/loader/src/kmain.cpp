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

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "kernel/system.h"
#include "kernel/video.h"
#include "kernel/multiboot.h"
#include "kernel/initrd.h"
#include "kernel/elf.h"
#include "kernel/acpi.h"
#include "kernel/frame.h"
#include "kernel/virtual.h"
#include "icxxabi.h"

System::Parameters g_system_params;

/**
 * This method is called right after stack is set to do the most
 * basic setup
 */
extern "C" void boot()
{
    Video::Install();
}


/**
 * Setup
 */
extern "C" uint32_t kmain(uint32_t magic, MBoot::InfoTable * info)
{
	// no magic
	if (magic != MBoot::BOOTLOADER_MAGIC || info == NULL)
	{
		printf("Invalid or no multiboot info\n");
		return 0;
	}

    // check memory maps
    size_t mcnt = info->GetMemMapCount();
    if (mcnt != 0)
    {
        info->DumpMemoryMap();
        for (size_t i = 0; i < mcnt; i++)
        {
            auto map = info->GetMemMap(i);
            if (map->GetType() == 1)        //Only worry about free memory
                Frame::AddRange(map->GetAddr(), map->GetLen());
        }
    }
    else
    {
        printf("No memory map provided");
        return 0;
    }

    //Save our lower 4kb area for real mode emulator in the kernel
    Frame::ReserveRange(0, 4096, FRAME_USED);

    // reserve grub multiboot main structure!
    Frame::ReserveRange(
        PTR_TO_U64(info),
        sizeof(MBoot::InfoTable),
        FRAME_AVAILABLE
    );

    // Check modules
    size_t cnt = info->GetModuleCount();
    if (cnt != 0)
    {
        printf("Found %d modules\n", cnt);
        info->DumpModules();
        for (size_t i = 0; i < cnt; i++)
        {

            auto module = info->GetModule(i);
            Frame::ReserveRange(
                PTR_TO_U64(module->GetStart()),
                (uint64_t)module->GetSize(),
                FRAME_USED
            );
        }

        // preserve modules table
        Frame::ReserveRange(
            PTR_TO_U64(info->GetModsTableAddr()),
            (uint64_t)info->GetModsTableSize(),
            FRAME_AVAILABLE
        );
    }
    else
    {
		printf("No modules loaded by multiboot.\n");
		return 0;
    }

    // reserve loader image
    extern uint32_t code asm("code");
    extern uint32_t end asm("end");

    Frame::ReserveRange(
        PTR_TO_U64(&code),
        PTR_TO_U64(&end) - PTR_TO_U64(&code),
        FRAME_AVAILABLE
    );

    //Lets startup our virtual memory manager
    VirtualMemory::Install();

    //Lets also map this loader 1:1
    VirtualMemory::MapRange
        (
            PTR_TO_U64(&code), PTR_TO_U64(&code),
            PTR_TO_U64(&end) - PTR_TO_U64(&code),
            VIRTUAL_FLAG_PRESENT | VIRTUAL_FLAG_READWRITE |
            VIRTUAL_FLAG_GLOBAL
        );

    auto initrd_module = info->FindModuleByCmdLine("initrd");
	if (initrd_module == NULL)
	{
	    printf("Module initrd not found\n");
	    return 0;
	}

    RdFileInfo file_info;
    g_initrd.Load((RdHeader *)initrd_module->GetStart());

    Acpi::Install();
//    return 0;

    uint64_t entry;
    // Load the kernel
    if  (g_initrd.FindFile("kernel.bin", &file_info) != INITRD_OK)
    {
        printf("Could not find kernel.bin in the initial ram disk!\n");
        return 0;
    }
    else
    {
        Elf64_Ehdr * elf = (Elf64_Ehdr *)file_info.data;
        entry = Elf::Load(elf);   //Load the elf file
    }

    //Make sure we set our magic # for our parameters
    g_system_params.magic = KERNEL_MAGIC;

    return entry;
}
