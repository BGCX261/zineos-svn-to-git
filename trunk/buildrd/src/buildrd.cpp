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


#include "../include/buildrd.h"

#include <string.h>


InitRd::InitRd(void)
{
    m_list_head = 0;
}


InitRd::~InitRd(void)
{
    Unload();
}


void InitRd::Unload(void)
{
    //Remove the list one item at a time
    while (m_list_head)
    {
        Remove(m_list_head->name_ptr);
    }
}


void InitRd::Display(void)
{
    RdListEntry *entry = m_list_head;
    if (entry==0)
        return;
    do
    {
        printf("%s - %d\n", entry->name_ptr, entry->file_size);
        entry = entry->next_ptr;
    } while (entry != m_list_head);
}


uint32_t InitRd::Remove(char *file_name)
{
    RdListEntry *entry = Find(file_name);
    if (entry == 0)
    {
        return INITRD_FILE_NOT_FOUND;
    }
    //Otherwise, lets properly remove it    {
    if (entry->name_ptr)
    {
        free(entry->name_ptr);
    }
    if (entry->data_ptr)
    {
        free(entry->data_ptr);
    }
    //Take me out of the list
    entry->next_ptr->prev_ptr = entry->prev_ptr;
    entry->prev_ptr->next_ptr = entry->next_ptr;
    if (m_list_head == entry)
    {
        m_list_head = m_list_head->next_ptr;
        if (m_list_head == entry)   //This was our only entry?
        {
            m_list_head = 0;
        }
        free(entry);
    }
    return INITRD_OK;
}


uint32_t InitRd::FindLast(char *var, char val)
{
    uint32_t loc=-1, ctr=0;
    while (*var)
    {
        if (*var == val)
            loc = ctr;
        ++var;
        ++ctr;
    }
    return loc;
}


RdListEntry *InitRd::Find(char *file_name)
{
    RdListEntry *entry = m_list_head;
    if (entry == 0)
    {
        return 0;
    }
    do
    {
        //Check for equal strings
        if (strcmp(file_name, entry->name_ptr) == 0)
        {
            return entry;
        }
        entry = entry->next_ptr;
    } while (entry != m_list_head);
    return 0;
}


uint32_t InitRd::AddFromMemory(char *name, void *mem_ptr, uint32_t size)
{
    RdListEntry *new_entry = Find(name);    //See if it exists already

    if (new_entry)      //Yup, we're replacing this one
    {
        free(new_entry->data_ptr);  //Free the data
    }
    else
    {
        new_entry = (RdListEntry*)malloc(sizeof(RdListEntry));
        new_entry->name_length = strlen(name) + 1;
        new_entry->name_ptr = (char*)malloc(new_entry->name_length);
        memcpy(new_entry->name_ptr, name, new_entry->name_length);
    }
    new_entry->file_size = size;
    new_entry->data_ptr = mem_ptr;  //We assume data is allocated!

    if (m_list_head == 0)     //First item in our list?
    {
        m_list_head = new_entry;
        m_list_head->next_ptr = m_list_head;
        m_list_head->prev_ptr = m_list_head;
    }
    else
    {
        new_entry->next_ptr = m_list_head;
        new_entry->prev_ptr = m_list_head->prev_ptr;
        m_list_head->prev_ptr->next_ptr = new_entry;
        m_list_head->prev_ptr = new_entry;
    }

    return INITRD_OK;
}


uint32_t InitRd::Load(char *file_name)
{
    RdFileEntry *file_entry_list;
    char         *symbol_table;

    FILE *in = fopen(file_name, "rb");
    if (in == 0) //File doesn't exist yet
    {
        return INITRD_FILE_NOT_FOUND;
    }
    fread(&m_header, 1, sizeof(RdHeader), in);  //Read in the header

    if (m_header.magic != INITRD_MAGIC)
    {
        return INITRD_INVALID_FILE;
    }
    if (m_header.version != INITRD_VERSION)
    {
        return INITRD_WRONG_VERSION;
    }

    //First setup symbol table
    symbol_table = (char*)malloc(m_header.symbol_size);
    //Read the symbol table
    fread(symbol_table, 1, m_header.symbol_size, in);

    //Allocate space for entries
    file_entry_list = (RdFileEntry*)malloc
        (
            sizeof(RdFileEntry) * m_header.entry_count
        );
    //First grab all the entries
    fread(file_entry_list, m_header.entry_count, sizeof(RdFileEntry), in);

    //Following is the actual files
    for (uint32_t i = 0; i != m_header.entry_count; ++i)
    {
        void *data = malloc(file_entry_list[i].file_size);
        fread(data, 1, file_entry_list[i].file_size, in);
        AddFromMemory
            (
                &symbol_table[file_entry_list[i].symbol_offset],
                data,
                file_entry_list[i].file_size
            );
    }

    free(file_entry_list);
    free(symbol_table);
    fclose(in);
    return INITRD_OK;
}


uint32_t InitRd::AddReplace(char *file_name)
{
    FILE *in = fopen(file_name, "rb");
    uint32_t size;
    void *data;
    char *save_name;
    if (in == 0)
    {
        return INITRD_FILE_NOT_FOUND;
    }

    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);

    save_name = &file_name[FindLast(file_name, '/')+1];
    data = malloc(size);
    fread(data, 1, size, in);
    fclose(in);

    return AddFromMemory(save_name, data, size);
}


uint32_t InitRd::Save(char *out_file)
{
    RdListEntry *entry = m_list_head;
    RdFileEntry file_entry;

    if (entry == 0)
    {
        return 0;
    }

    FILE *out = fopen(out_file, "wb");
    if (out == 0)
    {
        return INITRD_FILE_OPEN_ERROR;
    }

    m_header.magic = INITRD_MAGIC;
    m_header.version = INITRD_VERSION;
    m_header.entry_count = 0;
    m_header.symbol_size = 0;
    m_header.size = sizeof(RdHeader);
    do
    {
        m_header.entry_count ++;
        m_header.symbol_size += entry->name_length;
        m_header.size +=
            entry->file_size + entry->name_length + sizeof(RdFileEntry);
        entry = entry->next_ptr;
    } while (entry != m_list_head);

    //Our header should be 100% filled in
    fwrite(&m_header, 1, sizeof(RdHeader), out);

    //Now write out the symbol table
    entry = m_list_head;
    do
    {
        fwrite(entry->name_ptr, 1, entry->name_length, out);
        entry = entry->next_ptr;
    } while (entry != m_list_head);

    file_entry.file_offset = sizeof(RdHeader) + m_header.symbol_size;
    file_entry.file_offset += sizeof(RdFileEntry) * m_header.entry_count;
    file_entry.symbol_offset = 0;

    entry = m_list_head;
    do
    {
        file_entry.file_size = entry->file_size;
        fwrite(&file_entry, 1, sizeof(RdFileEntry), out);
        file_entry.symbol_offset += entry->name_length;
        file_entry.file_offset += file_entry.file_size;
        entry = entry->next_ptr;
    } while (entry != m_list_head);

    entry = m_list_head;
    do
    {
        fwrite(entry->data_ptr, 1, entry->file_size, out);
        entry = entry->next_ptr;
    } while (entry != m_list_head);

    fclose(out);
    return INITRD_OK;
}

