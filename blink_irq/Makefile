# =============================================================================
# blink_irq -- FreeRTOS blink + BTNC, interrupts via the AXI INTC.
#
#   make          -> build/app.elf, app.bin, app.mem (BRAM image), app.lst
#   make clean
#
# TOOLCHAIN
#   riscv-none-elf-gcc on PATH by default. Another prefix or location:
#       make CROSS=riscv64-unknown-elf-  TOOLDIR=/path/to/bin
# =============================================================================
TOOLDIR ?=
CROSS   ?= riscv-none-elf-

# Use TOOLDIR when given, otherwise whatever is on PATH.
ifneq ($(wildcard $(TOOLDIR)/$(CROSS)gcc.exe)$(wildcard $(TOOLDIR)/$(CROSS)gcc),)
  TC = $(TOOLDIR)/$(CROSS)
else
  TC = $(CROSS)
endif

CC       = $(TC)gcc
OBJCOPY  = $(TC)objcopy
OBJDUMP  = $(TC)objdump
SIZE     = $(TC)size
PYTHON  ?= python

BUILD   ?= build
RTOS    ?= ../freertos
TOOLS   ?= ../tools
PORT     = $(RTOS)/portable/GCC/RISC-V

# The xPack toolchain ships its own newlib headers, so nothing extra is needed.
# On a Linux box with picolibc, point this at its include directory.
LIBC_INC ?=

ARCH     = -march=rv32imc_zicsr -mabi=ilp32 -mcmodel=medany
CFLAGS   = $(ARCH) -O2 -g -ffunction-sections -fdata-sections \
           -Wall -Wextra -Wno-unused-parameter \
           -nostdlib -ffreestanding -fno-builtin \
           -I. -I$(RTOS)/include -I$(PORT) $(LIBC_INC) $(EXTRA_CFLAGS)
ASFLAGS  = $(ARCH) -I. -I$(RTOS)/include -I$(PORT) -D__ASSEMBLER__ \
           -DportasmHANDLE_INTERRUPT=freertos_risc_v_application_interrupt_handler
LDFLAGS  = $(ARCH) -nostdlib -nostartfiles -Wl,--gc-sections \
           -Wl,-T,link.ld -Wl,-Map,$(BUILD)/app.map

SRC_C    = main.c drivers.c intc.c libmini.c \
           $(RTOS)/tasks.c $(RTOS)/queue.c $(RTOS)/list.c \
           $(PORT)/port.c $(RTOS)/portable/MemMang/heap_4.c
SRC_S    = start.S $(PORT)/portASM.S

OBJ      = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(SRC_C))) \
           $(patsubst %.S,$(BUILD)/%.o,$(notdir $(SRC_S)))
VPATH    = .:$(RTOS):$(PORT):$(RTOS)/portable/MemMang

all: $(BUILD)/app.elf $(BUILD)/app.bin $(BUILD)/app.mem $(BUILD)/app.lst
	@$(SIZE) $(BUILD)/app.elf

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/app.elf: $(OBJ) link.ld
	$(CC) $(LDFLAGS) $(OBJ) -lgcc -o $@

$(BUILD)/app.bin: $(BUILD)/app.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/app.lst: $(BUILD)/app.elf
	$(OBJDUMP) -d -S $< > $@

# 64-bit little-endian words, one per line -- what soc_bram.v's $readmemh wants.
$(BUILD)/app.mem: $(BUILD)/app.bin
	$(PYTHON) $(TOOLS)/bin2mem.py $< --hex $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean
