# counter

A 4-bit counter on LD15..LD12, changed by three independent sources:

- **BTNU** / **BTND**: count up / down
- **UART console**: `inc`, `dec`, `set <0-15>`, `help`
- **BTNC** toggles LD1, and LD0 blinks at 1 Hz from a software timer.

Build with `make`; see the [top-level README](../README.md) for running it.

## Layout

| Folder | Contents |
|---|---|
| `app/` | `main.c` and one file per task |
| `app/utils/` | formatting and minimal libc helpers |
| `bsp/` | register access for GPIO, UART and the interrupt controller; no FreeRTOS |
| `rtos/` | FreeRTOS configuration, startup code, linker script |

| File | Role |
|---|---|
| `app/main.c` | creates the queue, the timer and the tasks, starts the scheduler |
| `app/counter.c` | task that owns the counter value and LD15..LD12 |
| `app/buttons.c` | task that turns button presses into counter events |
| `app/console.c` | task that parses commands from the UART |
| `app/io.c` | shared LED register and console output |
| `app/irq.c` | trap vector, interrupt handler, button debounce |

## How it works

All three sources post events to one queue, `q_count`. The counter task is
the only one that reads it, so it owns the counter and its LEDs without any
locking.

The other shared resources use the lightest protection that fits:

| Resource | Protection | Why |
|---|---|---|
| counter | queue | many producers, one owner |
| UART output | mutex | two tasks print; a line takes milliseconds |
| LED register | critical section | a read-modify-write of a few instructions |

The interrupt handler only records which button fired, masks the GPIO
interrupt and wakes the button task. The task waits 20 ms, re-reads the
buttons and re-arms the interrupt, which is the debounce.

## Notes

- The FreeRTOS RISC-V port does not set `mtvec`; `main()` does.
- Interrupts are enabled from inside the tasks, not in `main()`, so an
  interrupt left pending after reset cannot fire before the scheduler runs.
- `IP_ISR` in the AXI GPIO is toggle-on-write. It is cleared by reading it
  and writing back only the bits that are set.
