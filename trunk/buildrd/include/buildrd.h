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


#ifndef BUILDRD_H_INCLUDED
#define BUILDRD_H_INCLUDED

#define INITRD_OK               0
#define INITRD_FILE_NOT_FOUND   1
#define INITRD_OUT_OF_MEMORY    2
#define INITRD_FILE_OPEN_ERROR  3
#define INITRD_INVALID_FILE     4
#define INITRD_WRONG_VERSION    5

#define INITRD_MAGIC            'init'
#define INITRD_VERSION          0x00010000

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct RdHeader
{
    uint32_t    magic;              //This should be 'init'
    uint32_t    version;            //Version info
    uint32_t    size;               //Size of entire image
    uint32_t    entry_count;        //# of file entries
    uint32_t    symbol_size;        //Size of symbol table
};

struct RdFileEntry
{
    uint32_t    file_offset;        //Offset from start to end of file
    uint32_t    file_size;          //Length of name that follows structure
    uint32_t    symbol_offset;      //Offset to symbol
    uint32_t    reserved;           //Reserve some space for add-ons
};

struct RdListEntry
{
    char        *name_ptr;
    void        *data_ptr;
    uint32_t    name_length;
    uint32_t    file_size;
    RdListEntry *next_ptr;
    RdListEntry *prev_ptr;
};

class InitRd
{
public:
    InitRd(void);
    ~InitRd(void);
    uint32_t    AddReplace(char *file_name);
    uint32_t    Remove(char *file_name);
    uint32_t    Extract(char *folder);
    void        Display(void);
    uint32_t    Save(char *out_file);
    uint32_t    Load(char *file_name);
    void        Unload(void);

    RdHeader m_header;
    RdListEntry *m_list_head;
private:
    uint32_t FindLast(char *var, char val);
    uint32_t AddFromMemory(char *name, void *mem_ptr, uint32_t size);
    RdListEntry *Find(char *file_name);
};

#endif
