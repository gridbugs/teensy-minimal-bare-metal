TOOL_PREFIX=arm-none-eabi
CC=$(TOOL_PREFIX)-gcc
OBJCOPY=$(TOOL_PREFIX)-objcopy
LOADER=teensy-loader-cli

SRC_DIR=src
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:.c=.o)

LIBS=-lgcc

CFLAGS=-O3 -Wall -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16
LDFLAGS=-nostdlib -nostartfiles -Timxrt1062.ld

OUTFILE=firmware

$(OUTFILE).hex: $(OUTFILE).elf
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

$(OUTFILE).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tags: generate_tags.sh $(wildcard $(SRC_DIR)/*.[ch])
	./$<

flash: $(OUTFILE).hex
	$(LOADER) --mcu=TEENSY40 -w -v $<

clean:
	rm -rf $(SRC_DIR)/*.o *.elf *.hex

.PHONY: clean flash
