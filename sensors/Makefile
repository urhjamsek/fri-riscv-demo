# =============================================================================
# sensors -- the counter demo plus the on-board I2C and SPI sensors.
#
#   make          -> build/app.elf, app.bin, app.mem (BRAM image), app.lst
#   make clean
#
#   app/        main, one file per task, shared LEDs and console
#   app/utils/  formatting and minimal libc helpers
#   bsp/        register access, no FreeRTOS
#   rtos/       FreeRTOS configuration, startup code, linker script
#
# The FreeRTOS kernel (submodule) and bin2mem.py are shared by all demos.
# Override with:  make RTOS=/path/to/freertos
# =============================================================================
CROSS   ?= riscv-none-elf-
CC       = $(CROSS)gcc
OBJCOPY  = $(CROSS)objcopy
OBJDUMP  = $(CROSS)objdump
SIZE     = $(CROSS)size

BUILD   ?= build
RTOS    ?= ../freertos
TOOLS   ?= ../tools
PYTHON  ?= python
PORT     = $(RTOS)/portable/GCC/RISC-V

LIBC_INC ?=

ARCH     = -march=rv32imc_zicsr -mabi=ilp32 -mcmodel=medany
INCS     = -Iapp -Iapp/utils -Ibsp -Irtos -I$(RTOS)/include -I$(PORT)
CFLAGS   = $(ARCH) -O2 -g -ffunction-sections -fdata-sections \
           -Wall -Wextra -Wno-unused-parameter \
           -nostdlib -ffreestanding -fno-builtin \
           $(INCS) $(LIBC_INC) $(EXTRA_CFLAGS)
ASFLAGS  = $(ARCH) $(INCS) -D__ASSEMBLER__ \
           -DportasmHANDLE_INTERRUPT=freertos_risc_v_application_interrupt_handler
LDFLAGS  = $(ARCH) -nostdlib -nostartfiles -Wl,--gc-sections \
           -Wl,-T,rtos/link.ld -Wl,-Map,$(BUILD)/app.map

APP_C    = app/main.c app/counter.c app/buttons.c app/console.c app/io.c app/irq.c
UTIL_C   = app/utils/fmt.c app/utils/libmini.c
BSP_C    = bsp/gpio.c bsp/uart.c bsp/intc.c bsp/temp.c bsp/accel.c
RTOS_C   =            $(RTOS)/tasks.c $(RTOS)/queue.c $(RTOS)/list.c $(RTOS)/timers.c \
           $(PORT)/port.c $(RTOS)/portable/MemMang/heap_4.c
SRC_C    = $(APP_C) $(UTIL_C) $(BSP_C) $(RTOS_C)
SRC_S    = rtos/start.S $(PORT)/portASM.S

OBJ      = $(patsubst %.c,$(BUILD)/%.o,$(notdir $(SRC_C))) \
           $(patsubst %.S,$(BUILD)/%.o,$(notdir $(SRC_S)))
VPATH    = app:app/utils:bsp:rtos:$(RTOS):$(PORT):$(RTOS)/portable/MemMang

all: $(BUILD)/app.elf $(BUILD)/app.bin $(BUILD)/app.mem $(BUILD)/app.lst
	@$(SIZE) $(BUILD)/app.elf

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/app.elf: $(OBJ) rtos/link.ld
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
