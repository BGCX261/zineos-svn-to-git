#
# makefile to build ZineOS kernel
#

GCC_PREFIX	= x86_64-elf-

# The output file
BIN		= ../bin/trampoline.bin

#Linker
LD		= $(GCC_PREFIX)ld
LDFLAGS		= -T link.ld -nostdlib -nodefaultlibs

# NASM assembler and files
ASM		= nasm
ASMFLAGS	= -f elf32
ASMMAIN		= trampoline.asm
ASMSRCS		= src/$(ASMMAIN) $(filter-out src/$(ASMMAIN), $(wildcard src/*.asm)) \
		  $(wildcard src/mm/*.asm)

# C++ compiler and files
CPP			= $(GCC_PREFIX)g++
CPPINCS		= -I./include/libc -I./include
CPPFLAGS	= $(CPPINCS) -m32 -fleading-underscore --std=gnu++0x \
		  -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
		  -Wall -Wextra -ffreestanding -nostdlib -fno-builtin -nostartfiles \
		  -nodefaultlibs -fno-stack-protector -fno-rtti -nostdinc++ \
		  -fno-nonansi-builtins -fno-exceptions \
		  -Os
CPPSRCS		= $(wildcard src/*.cpp)

# ASM Files *MUST* be first (ASMMAIN)
OBJECTS		= $(patsubst src/%.asm,obj/%.asm.o,$(ASMSRCS))\
		  $(patsubst src/%.cpp,obj/%.cpp.o,$(CPPSRCS))

# Build targets
.PHONY : all clean link

# Rule for ALL
all : $(BIN)

# Rule for CLEAN
clean :
	rm -f $(OBJECTS) $(BIN)

link:
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJECTS)

# Compile C sources
obj/%.cpp.o : src/%.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

# Compile ASM sources
obj/%.asm.o : src/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# link together and create the binary
$(BIN) : $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJECTS)

