# sensors

The [`counter`](../counter) demo, plus the two sensors on the Nexys A7:

- `temp`: temperature from the ADT7420, over I2C
- `accel`: X/Y/Z acceleration from the ADXL362, over SPI

Everything else (buttons, LEDs, `inc` / `dec` / `set`) behaves as in
`counter`. Build with `make`; see the [top-level README](../README.md) for
running it.

The sensor drivers are `bsp/temp.c` and `bsp/accel.c`. Both are used only by
the console task, so they need no locking.
