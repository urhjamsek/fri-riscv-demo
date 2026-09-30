/*=============================================================================
 * start.S -- reset entry.
 *
 * The whole image is preloaded into RAM, so there is no data copy.
 * Sets gp and sp, installs a placeholder mtvec, zeroes .bss, calls main.
 *===========================================================================*/
    .section .text.init
    .global _start
_start:
.option push
.option norelax
    la      gp, __global_pointer$
.option pop
    la      sp, __stack_top

    /* Until the scheduler installs its own, any trap parks here. */
    la      t0, _early_trap
    csrw    mtvec, t0

    /* Zero .bss */
    la      t0, __bss_start
    la      t1, __bss_end
1:  bgeu    t0, t1, 2f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       1b
2:
    call    main
3:  j       3b

    .align 4
_early_trap:
    j       _early_trap
