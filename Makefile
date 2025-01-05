# Define variables
CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib
SRC_DIR = src
BUILD_DIR = build
KERNEL_DIR = $(SRC_DIR)/kernel
OBJS = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/stdio.o $(BUILD_DIR)/multitasking.o
LINKER_SCRIPT = $(SRC_DIR)/linker.ld
OUTPUT_BIN = $(BUILD_DIR)/memeos.bin

# Default target
all: $(OUTPUT_BIN)

# Assemble boot.s
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.s
	$(AS) $< -o $@

# Compile kernel.c
$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Compile memory.c
$(BUILD_DIR)/memory.o: $(KERNEL_DIR)/memory.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/stdio.o: $(KERNEL_DIR)/stdio.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR)/multitasking.o: $(KERNEL_DIR)/multitasking.c
	$(CC) -c $< -o $@ $(CFLAGS)


# Link all object files into the final binary
$(OUTPUT_BIN): $(OBJS)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(OBJS) $(LDFLAGS) -lgcc

# Clean build files
clean:
	rm -rf $(BUILD_DIR)/*.o $(OUTPUT_BIN)
