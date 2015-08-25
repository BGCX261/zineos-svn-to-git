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
#include "kernel/initrd.h"
#include "string.h"
#include "stdio.h"

//This is a global initrd so we can access it when initrd.h is included
InitRd g_initrd;


/**
 * Initialize everything to NULL
 */
InitRd::InitRd(void)
{
    m_header = 0;
}


/**
 * Load the initrd from the passed in address
 */
uint64_t InitRd::Load(RdHeader *header)
{
    m_header = header;

    if (m_header == 0)
    {
        return INITRD_FILE_NOT_FOUND;
    }
    if (m_header->magic != INITRD_MAGIC)
    {
        return INITRD_INVALID_FILE;
    }
    if (m_header->version != INITRD_VERSION)
    {
        return INITRD_WRONG_VERSION;
    }

    m_symbol_table = (char*)((size_t)m_header + sizeof(RdHeader));

    m_entry_list =
        (RdFileEntry *)((size_t)m_symbol_table + header->symbol_size);

    return INITRD_OK;
}

/**
 * Grab file information by index
 */
uint64_t InitRd::GetFile(uint64_t index, RdFileInfo *file_info)
{
    if (m_header == 0)
    {
        return INITRD_FILE_NOT_FOUND;   //Failed to find the init rd
    }
    if (index >= m_header->entry_count)
    {
        return INITRD_FILE_NOT_FOUND;   //not that many files!
    }

    file_info->data =
        (void*)((size_t)m_header + m_entry_list[index].file_offset);
    file_info->size =
        (size_t)m_entry_list[index].file_size;
    file_info->name = &m_symbol_table[m_entry_list[index].symbol_offset];

    return INITRD_OK;
}


/**
 * Get the number of files in the list
 */
uint64_t InitRd::GetFileCount(void)
{
    if (m_header == 0)
    {
        return 0;
    }
    return m_header->entry_count;
}


/**
 * Locates a file by name, useful :)
 */
uint64_t InitRd::FindFile(char *name, RdFileInfo *file_info)
{
    if(m_header == 0)
        return INITRD_FILE_NOT_FOUND;

    RdFileInfo tmp_info;

    for (uint32_t i=0; i!=m_header->entry_count; ++i)
    {
        GetFile(i, &tmp_info);
        if (strcmp(name, tmp_info.name) == 0)  //Got a match?
        {
            memcpy(file_info, &tmp_info, sizeof(RdFileInfo));
            return INITRD_OK;
        }
    }
    return INITRD_FILE_NOT_FOUND;
}
