# fri-riscv-demo

FreeRTOS demo programs for [fri-riscv-core](https://github.com/urhjamsek/fri-riscv-core),
a 32-bit RISC-V processor (RV32IMC_Zicsr) running on a Nexys A7-100T.

## Demos

| Demo | What it shows |
|---|---|
| [`blink_irq`](blink_irq) | The minimum: LD0 blinks from the FreeRTOS tick, BTNC toggles LD1 through the interrupt controller. |
| [`counter`](counter) | Three tasks sharing a 4-bit LED counter, changed by buttons and by commands over the UART. |
| [`sensors`](sensors) | `counter` plus the on-board temperature sensor (I2C) and accelerometer (SPI). |

## Requirements

- A RISC-V GCC toolchain. The Makefiles expect the `riscv-none-elf-` prefix
  ([xPack](https://xpack-dev-tools.github.io/riscv-none-elf-gcc-xpack/));
  use `make CROSS=riscv64-unknown-elf-` for another one.
- GNU make and Python 3.
- The FreeRTOS kernel, included as a submodule. Clone with:


## Building

```
cd <project>
make
```

This produces `build/app.mem`, the RAM image for the SoC.

## Running on the board

Either bake the image into the bitstream, as described in the
[core's README](https://github.com/urhjamsek/fri-riscv-core#running-it),

or upload the bitstream to the board over JTAG without rebuilding:

```tcl
set PROG <path>/counter/build/app.mem
source SoC/bd/load_prog.tcl
```

The console is on the board's USB serial port at 115200 baud.

