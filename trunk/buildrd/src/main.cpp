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

void DisplayUsage(void)
{
    printf("Usage:\tinitrd [options]\n");
    printf("\t-f [filename]\tSet initrd image file name\n");
    printf("\t-d\t\tDisplay files within initrd image\n");
    printf("\t-a [filename]\tAdd/Replace file\n");
    printf("\t-r [filename]\tRemove filename\n");
    printf("\t-e [folder]\tExtract to folder (folder optional)\n");
}


int main(int argc, char *argv[])
{
    InitRd init_rd;
    char *ramdisk_file = 0;
    uint32_t ret;

    if (argc<2)
    {
        DisplayUsage();
        return 0;
    }

    for (uint32_t i = 1; i != argc; ++i)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            if (i<argc-1)
            {
                ramdisk_file = argv[i+1];
                ++i;
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            DisplayUsage();
            return 0;
        }
    }

    if (ramdisk_file == 0)
    {
        ramdisk_file = "initrd.bin";
        printf("Warning, no output file specified, defaulting to 'initrd.bin'\n");
    }

    ret = init_rd.Load(ramdisk_file);
    if (ret == INITRD_INVALID_FILE)
    {
        printf("Magic value did not match\n");
        return 0;
    }
    else if (ret == INITRD_WRONG_VERSION)
    {
        printf("Not compatible with this version, need to rebuild the file\n");
        printf("Nothing was done.\n");
        return 0;
    }

    for (uint32_t i = 1; i != argc; ++i)
    {
        if (strcmp(argv[i], "-a") == 0)
        {
            if (i<argc-1)
            {
                uint32_t ret;
                ret = init_rd.AddReplace(argv[i+1]);
                if (ret == INITRD_FILE_NOT_FOUND)
                {
                    printf("Couldn't find %s\n", argv[i+1]);
                }
                ++i;
            }
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            //Remove a file
            if (i<argc-1)
            {
                uint32_t ret;
                ret = init_rd.Remove(argv[i+1]);
                if (ret == INITRD_FILE_NOT_FOUND)
                {
                    printf("Couldn't find file to remove %s\n", argv[i+1]);
                }
                ++i;
            }
        }
        else if (strcmp(argv[i], "-e") == 0)
        {
            printf("Export not implemented yet, sorry!\n");
            return 0;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            init_rd.Display();
            return 0;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            ++i;
        }
        else
        {
            printf("Unrecognized command %s\n", argv[i]);
            DisplayUsage();
            return 0;
        }
    }

    init_rd.Save(ramdisk_file);

    return 0;
}
