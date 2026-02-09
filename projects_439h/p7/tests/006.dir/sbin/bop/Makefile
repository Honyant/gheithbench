# Define paths
OTHER_DIR = ..

# Save original PATH and LD_LIBRARY_PATH
ORIGINAL_PATH := $(PATH)
ORIGINAL_LD_LIBRARY_PATH := $(LD_LIBRARY_PATH)

# Set PATH explicitly
PATH := /usr/bin
GCC := /usr/bin/gcc

# Define utilities and source/object files
UTILS = fork1 fork2  # These will be created in the current directory
LIB_SOURCES = $(OTHER_DIR)/libc.c $(OTHER_DIR)/sys.c $(OTHER_DIR)/crt0.c \
              $(OTHER_DIR)/heap.c $(OTHER_DIR)/machine.c $(OTHER_DIR)/printf.c
OFILES = $(LIB_SOURCES:.c=.o)

CFLAGS = -std=c99 -m32 -nostdlib -O2 -Wall -Werror -I$(OTHER_DIR)

all : $(UTILS)
	export PATH=$(ORIGINAL_PATH)
	export LD_LIBRARY_PATH=$(ORIGINAL_LD_LIBRARY_PATH)

.SECONDARY :

# Compile .c files in OTHER_DIR to .o files
$(OTHER_DIR)/%.o : $(OTHER_DIR)/%.c
	$(GCC) -c -MD $(CFLAGS) -o $@ $<

# Compile fork1 and fork2 in the current directory
%.o : %.c
	$(GCC) -c -MD $(CFLAGS) $< -o $@

# Linking rule for each utility, linking with object files from OTHER_DIR
$(UTILS) : % : %.o $(OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $< $(OFILES)

# Clean rule
clean ::
	rm -f *.o
	rm -f *.d
	rm -f $(UTILS)

-include *.d

.PHONY: all clean
