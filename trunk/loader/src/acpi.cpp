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


#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "kernel/system.h"
#include "kernel/acpi.h"
#include "kernel/initrd.h"
#include "kernel/frame.h"

/**
 * Performs a checksum on a range of memory for Acpi tables
 */
uint8_t Acpi::CheckSum(uint8_t *table_ptr, uint32_t length)
{
        uint8_t val=0;
        while (length--)
            val += *table_ptr++;
        return val;
}


/**
 * Used to find the Rsdp in memory and return it's address
 * Searches bios area, then ebda.
 */
Acpi::Rsdp * Acpi::FindRsdp(void)
{
    uint32_t offset, addr;

    //First search the bios section, 16-byte aligned
    for (offset = 0xe0000; offset < 0xfffff;offset += 16)
    {
        //Does our signature match?
        if (memcmp("RSD PTR ",(uint8_t*)offset,8) == 0)
        {
//            printf("RSDP Possible: %p\n", offset);
            //Verify our checksum
            if (!CheckSum((uint8_t*)offset, 20))
            {
                return (Acpi::Rsdp *)offset;                  //We found it!
            }
        }
    }

    //If we couldn't find it in the normal BIOS section, lets search EBDA
    offset = *(uint16_t*)0x040E;    //16-bit Segment
    offset <<= 4;                   //Shift to get physical addresss

    //0x400 = size of EBDA, search 16-byte boundary
    for (addr = 0; addr < 0x400; addr += 16)
    {
        //Does our signature match?
        if (memcmp("RSD PTR ",(uint8_t*)offset,8) == 0) //So far so good
        {
            //If good signature, check our checksum
            if (!CheckSum((uint8_t*)offset, 20))
            {
                return (Acpi::Rsdp *)offset;
            }
        }
        //Add 16 to the current offset
        offset += 16;
    }
    return (Acpi::Rsdp *)0;   //Crap, nothing was found
}


/**
 * Send out a startup IPI
 */
void Acpi::SendApicCommand(uint32_t apic_address, uint32_t location, uint32_t command)
{
    uint32_t volatile *sipi_ptr = (uint32_t*)(apic_address + location);
    uint32_t timeout=65535;

//    printf("Sending command %p - %p\n", sipi_ptr, command);
    *sipi_ptr = command;            //Send the command!

//Await a response
    while ((*sipi_ptr & (1<<12)) && timeout--)
    {
    }
}


/**
 * Used to Parse the Madt table, find's Io Apic and all Cpu's
 */
 void Acpi::ParseMadt(Madt *madt_ptr)
{
	size_t  ctr;
	size_t  cpu_count=0;
	size_t  offset;

    //Get the offset of the pMadt
	offset = (uint32_t)madt_ptr;
	//Nowadd it's size, to get past it
	offset+= sizeof(Acpi::Madt);

    //start our counter at size
	ctr = sizeof(Acpi::Madt);

    printf(
        "MADT->Length: %d, Size: %d, Revision %d, APIC Address 0x%x\n",
        madt_ptr->length, sizeof(Acpi::Madt),
        madt_ptr->revision,madt_ptr->local_apic_addr
        );


    //Loop  until we hit the end
	while (ctr < madt_ptr->length)
	{
        auto *sub_table_ptr = (Acpi::MadtSubTable *)offset;

		//Check  the type of sub table
		if (sub_table_ptr->type == LOCAL_APIC)
		{
		    //Local Apic, aka a Cpu was foudn
            auto acpi_local_apic_ptr = (Acpi::LocalApic*)offset;

			++cpu_count;
			printf(
                "Found CPU #%d, Local APIC ID %d, Flags 0x%x\n",
                cpu_count, acpi_local_apic_ptr->local_apic_id,
                acpi_local_apic_ptr->flags
                );
		}
		else if (sub_table_ptr->type == IO_APIC)
		{
		    //Io Apic was found
            auto acpi_io_apic_ptr = (Acpi::IoApic*)offset;

			printf(
                "Found IO APIC @ 0x%x, IRQ Base %d, ID %d\n",
                acpi_io_apic_ptr->address, acpi_io_apic_ptr->irq_base,
                acpi_io_apic_ptr->io_apic_id
                );
		}
		else if (sub_table_ptr->type == INTERRUPT_SOURCE_OVERRIDE)
		{
            auto *iso_ptr = (Acpi::InterruptSourceOverride*)offset;
		    //Found an interrupt source override
            printf(
                "Int Override: Normal 0x%x -> Global 0x%x\n",
                iso_ptr->source, iso_ptr->global_source_interrupt
                );
		}
		else    //Not yet supported...
		{
		    //not sure what this one is, lets ignore it
		}

        //Skip this tables length, so even if we don't know what
        //was in it, we can just skip it
		offset += sub_table_ptr->length;
		ctr += sub_table_ptr->length;
		//If length = 0, lets get out of here
		if (sub_table_ptr->length == 0)
		{
			break;
		}
	}

	if (cpu_count > 0)
	{
        RdFileInfo file_info;
        Acpi::Trampoline *trampoline;

	    //Lets allocate some space sub 1mb first, for our trampoline
        if (g_initrd.FindFile("trampoline.bin", &file_info) != INITRD_OK)
        {
            printf("Could not find trampoline.bin, skipping ACPI\n");
            return;
        }

        trampoline = (Acpi::Trampoline *)Frame::Alloc
            (
                FRAME_SUB_1MB, 4096, FRAME_AVAILABLE
            );
        memcpy(trampoline, file_info.data, file_info.size);

        trampoline->cpu_count = 1;   //BP already set
        trampoline->gdt.location += (uint32_t)trampoline;

        extern uint32_t ApplicationProcessor asm("ApplicationProcessor");
        trampoline->address = (uint32_t)&ApplicationProcessor;

        printf("AP Start Address: 0x%x\n", trampoline->address);

        //Send INIT
        Acpi::SendApicCommand(madt_ptr->local_apic_addr, ICR_LOW, 0xC4500);
//    Acpi::SendApicCommand(madt_ptr->local_apic_addr, ICR_LOW, 0xC4600 + 0x01);
        //Send IPI with 4kb page number
        Acpi::SendApicCommand
            (
                madt_ptr->local_apic_addr,
                ICR_LOW, 0xC4600 + (((uint32_t)trampoline) >> 12)
            );

        uint32_t counter=0xffffff;

        //TODO:  Fix me just incase someone doesn't start up!
        while (*(volatile uint32_t *)&trampoline->cpu_count != cpu_count)
        {
            if (counter == 0)
                break;
            --counter;
        }

        uint32_t *p_tmp = (uint32_t*)0x1003;
        //Let our memory go... :)
        Frame::Free(trampoline);
        printf("CPU's Running: 0x%x\n", *p_tmp);//trampoline->cpu_count);
    }
}


/**
 * Used by Install to Parse the Rsdp Table
 */
void Acpi::ParseRsdp(Rsdp *rsdp_ptr)
{
    auto *rsdt_ptr = (Acpi::Rsdt*)rsdp_ptr->address;
	uint32_t table_count;
	uint32_t *offset_ptr;

    //Make sure we have a valid checksum
	if (Acpi::CheckSum((uint8_t*)rsdt_ptr, rsdt_ptr->length))
	{
		return;								        //Error with RSDT!
	}

    //Get the number of tables
	table_count = (rsdt_ptr->length-sizeof(Acpi::Rsdt)) / 4;
	//Startafter rsdt
	offset_ptr = (uint32_t*)(rsdp_ptr->address + 36);

    //Lets find the Madt
	for (uint32_t i = 0; i != table_count; ++i)
	{
        auto *table_ptr = (Acpi::Table*)offset_ptr[i];

		//Is this our APIC Table (Madt)
		if (memcmp(table_ptr->sig, "APIC", 4) == 0)
		{
			Acpi::ParseMadt((Acpi::Madt*)table_ptr);
		}
	}
}


/**
 * Scan the Acpi tables and initialize multiple Cpu's
 */
void Acpi::Install(void)
{
    Acpi::Rsdp *rsdp_ptr = Acpi::FindRsdp();

    if  (rsdp_ptr)
    {
        Acpi::ParseRsdp(rsdp_ptr);
    }
    else
    {
        printf("Acpi: Could not find Rsdp\n");
    }
}
