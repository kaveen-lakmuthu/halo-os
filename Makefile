# Halo OS - Main Makefile

# Tools
CC = x86_64-elf-gcc
LD = x86_64-elf-ld
ASM = nasm

# Flags
CFLAGS = -ffreestanding -mno-red-zone -m64 -mcmodel=large -Isrc/kernel/include -g -Wall -Wextra
LDFLAGS = -n -nostdlib -T src/kernel/arch/x86_64/linker.ld

# Sources
# Find all .c and .asm files in src/kernel
C_SOURCES = $(shell find src/kernel -name "*.c")
ASM_SOURCES = $(shell find src/kernel -name "*.asm")

# Objects
OBJ = $(patsubst src/%.c, build/%.o, $(C_SOURCES)) \
      $(patsubst src/%.asm, build/%.o, $(ASM_SOURCES))

# Targets
.PHONY: all clean run iso

all: distro/halo-os.iso

# Link the kernel
build/kernel.bin: $(OBJ)
	@mkdir -p $(dir $@)
	@echo "  LD      $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJ)

# Compile C
build/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM
build/%.o: src/%.asm
	@mkdir -p $(dir $@)
	@echo "  ASM     $<"
	@$(ASM) -f elf64 $< -o $@

# Create ISO
distro/halo-os.iso: build/kernel.bin
	@echo "  ISO     $@"
	@cp build/kernel.bin distro/boot/
	@grub-mkrescue -o distro/halo-os.iso distro 2> /dev/null

run: distro/halo-os.iso
	@qemu-system-x86_64 -cdrom distro/halo-os.iso -m 2G -serial stdio

clean:
	rm -rf build distro/halo-os.iso distro/boot/kernel.bin