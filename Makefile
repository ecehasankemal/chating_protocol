# Define compilers and flags
CC = gcc
AS = nasm
CFLAGS = -Wall -Wextra -g
ASFLAGS = -f elf64
LDFLAGS = -no-pie -z noexecstack  # Linker flags

# Define directories
SRCDIR = srcs/ccode
ASMDIR = srcs/assembly
INCDIR = inc
OBJDIR = obj

# Define target executable
TARGET = chat_app

# Define source files
CSRC = $(SRCDIR)/rsa_eliptik_eğri_kriptografisi.c
ASM_SRC = $(ASMDIR)/my_read.asm $(ASMDIR)/my_strlen.asm $(ASMDIR)/my_write.asm

# Define object files
COBJ = $(OBJDIR)/rsa_eliptik_eğri_kriptografisi.o
ASM_OBJ = $(OBJDIR)/my_read.o $(OBJDIR)/my_strlen.o $(OBJDIR)/my_write.o

# All object files
OBJS = $(COBJ) $(ASM_OBJ)

# Default target
all: $(OBJDIR) $(TARGET)

# Create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile the C source files
$(COBJ): $(CSRC) | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Assemble the assembly files
$(ASM_OBJ): $(ASM_SRC) | $(OBJDIR)
	$(AS) $(ASFLAGS) $(ASMDIR)/my_read.asm -o $(OBJDIR)/my_read.o
	$(AS) $(ASFLAGS) $(ASMDIR)/my_strlen.asm -o $(OBJDIR)/my_strlen.o
	$(AS) $(ASFLAGS) $(ASMDIR)/my_write.asm -o $(OBJDIR)/my_write.o

# Link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)  # Use LDFLAGS here

# Clean the build directory
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

# Phony targets
.PHONY: all clean
