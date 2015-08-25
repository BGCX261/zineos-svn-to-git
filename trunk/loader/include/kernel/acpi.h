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

#ifndef ACPI_H_INCLUDED
#define ACPI_H_INCLUDED


//These are all based on the APIC Base Address
#define ICR_LOW     0x0300
#define APIC_ID     0x0020

namespace Acpi
{
    /**
     * ACPI Specified types for Madt Sub Tables
     */
    enum MadtType : uint8_t
    {
        LOCAL_APIC = 0,
        IO_APIC= 1,
        INTERRUPT_SOURCE_OVERRIDE = 2
    };

    /**
     * ACPI Specified Table Stub
     */
    struct Table
    {
        uint8_t     sig[4];
        uint32_t    length;
        uint8_t     revision;
        uint8_t     checksum;
    };

    /**
     * ACPI Specified RSDP
     */
    struct Rsdp
    {
        uint8_t     sig[8];
        uint8_t     checkSum;
        uint8_t     oem_id[6];
        uint8_t     revision;
        uint32_t    address;
    };

    /**
     * ACPI Specified RSDT
     */
    struct Rsdt
    {
        uint8_t     sig[4];
        uint32_t    length;
        uint8_t     revision;
        uint8_t     check_sum;
        uint8_t     oem_id[6];
        uint8_t     oem_table_id[8];
        uint32_t    oem_revision;
        uint8_t     creator_id[4];
        uint32_t    creator_revision;
    };

    /**
     * ACPI Specified MADT
     */
    struct Madt
    {
        uint8_t     sig[4];
        uint32_t    length;
        uint8_t     revision;
        uint8_t     check_sum;
        uint8_t     oem_id[6];
        uint8_t     oem_table_id[8];
        uint32_t    oem_revision;
        uint8_t     creator_id[4];
        uint32_t    creator_revision;
        uint32_t    local_apic_addr;
        uint32_t    flags;
    };

    /**
     * ACPI Specified sub table stub for Madt
     */
    struct MadtSubTable
    {
        Acpi::MadtType      type;
        uint8_t             length;
    };

    /**
     * ACPI Specified Local Apic (aka, CPU)
     */
    struct LocalApic
    {
        uint8_t     type;
        uint8_t     length;
        uint8_t     acpi_id;
        uint8_t     local_apic_id;
        uint32_t    flags;
    };

    /**
     * ACPI Specified Input Output APIC
     */
    struct IoApic
    {
        uint8_t     type;
        uint8_t     length;
        uint8_t     io_apic_id;
        uint8_t     reserved;
        uint32_t    address;
        uint8_t     irq_base;
    };

    /**
     * ACPI Specified Interrupt Source Override
     */
    struct InterruptSourceOverride
    {
        uint8_t     type;
        uint8_t     length;
        uint8_t     bus;
        uint8_t     source;
        uint32_t    global_source_interrupt;
        uint16_t    flags;
    };

    /**
     * This is our 32-bit GDT
     */
    struct Gdt
    {
        uint16_t size;
        uint32_t location;
        uint16_t null;
        uint64_t data_entry;
        uint64_t code_entry;
    }  __attribute__ ((__packed__));

    /**
     * This is the header for our Trampoline file
     */
    struct Trampoline
    {
        uint8_t     jump[3];    //Our jump instruction
        uint32_t    cpu_count;  //# of CPU's that have run this code
        uint32_t    address;    //offset into file jumping to pmode goes
        uint32_t    code_seg;   //code segment
        uint8_t     blank;
        Gdt         gdt;
    }  __attribute__ ((__packed__));


    /**
     * Performs a checksum on a range of memory for Acpi tables
     */
    uint8_t CheckSum(uint8_t *, uint32_t );


    /**
     * Used to find the Rsdp in memory and return it's address
     * Searches bios area, then ebda.
     */
    Rsdp  *FindRsdp(void);


    void SendApicCommand(uint32_t apic_address, uint32_t location, uint32_t command);

    /**
     * Used by Init to Parse the Rsdp Table
     */
    void ParseRsdp(Rsdp *);


    /**
     * Used to Parse the Madt table, find's Io Apic and all Cpu's
     */
    void ParseMadt(Madt *);


    /**
     * Scan the Acpi tables and initialize multiple Cpu's
     */
    void Install(void);
};
#endif
