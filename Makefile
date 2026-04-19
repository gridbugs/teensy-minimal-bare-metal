TOOL_PREFIX=arm-none-eabi
CC = $(TOOL_PREFIX)-gcc
LD = $(TOOL_PREFIX)-ld
OBJCOPY = $(TOOL_PREFIX)-objcopy
OBJDUMP = $(TOOL_PREFIX)-objdump
SIZE = $(TOOL_PREFIX)-size
LOADER = teensy_loader_cli

OUTFILE = firmware

BUILD_DIR = ./build
SRC_DIRS ?= ./src ./teensy ./include

SRCS := $(shell find $(SRC_DIRS) -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

LIBS = -lgcc

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS = -O3 -Wall -mcpu=cortex-m7 -mthumb -mfloat-abi=hard $(INC_FLAGS)
LDFLAGS = -nostdlib -nostartfiles -Tteensy/imxrt1062.ld

$(BUILD_DIR)/$(OUTFILE).hex: $(BUILD_DIR)/$(OUTFILE).elf
	@$(OBJCOPY) -O ihex -R .eeprom build/$(OUTFILE).elf build/$(OUTFILE).hex
	@$(OBJDUMP) -d -x build/$(OUTFILE).elf > build/$(OUTFILE).dis
	@$(OBJDUMP) -d -S -C build/$(OUTFILE).elf > build/$(OUTFILE).lst
	@$(SIZE) build/$(OUTFILE).elf

$(BUILD_DIR)/$(OUTFILE).elf: $(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.s.o: %.s
	@$(MKDIR_P) $(dir $@)
	@$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: flash
flash: $(BUILD_DIR)/$(OUTFILE).hex
	$(LOADER) --mcu=TEENSY40 -w -v $<

.PHONY: clean
clean:
	@$(RM) -r $(BUILD_DIR)

.PHONY: tags
tags: generate_tags.sh $(wildcard $(SRC_DIR)/*.[ch])
	./$<

MKDIR_P ?= mkdir -p
