# blink_irq

The smallest test of the interrupt path:

- LD0 blinks at 1 Hz from the FreeRTOS tick (CLINT timer interrupt).
- BTNC toggles LD1 through the GPIO, the AXI interrupt controller and `meip`.

If LD0 blinks but BTNC does nothing, the timer works and the external
interrupt path does not.

Build with `make`; see the [top-level README](../README.md) for running it.
