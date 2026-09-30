/*=============================================================================
 * intc.c -- AXI INTC (PG099).
 *
 * The INTC drives meip and is disabled after reset until MER is written.
 *   IER  one enable bit per input
 *   MER  ME (bit 0) enables the INTC, HIE (bit 1) enables hardware inputs
 *
 * Inputs are level-sensitive: clear the interrupt at the peripheral before
 * writing IAR, or the input is latched again.
 *===========================================================================*/
#include "soc.h"
#include "intc.h"

void intc_init(uint32_t mask)
{
    /* Enable the inputs, then the controller. */
    reg_wr(INTC_BASE, INTC_IER, mask);
    reg_wr(INTC_BASE, INTC_MER, INTC_MER_ME | INTC_MER_HIE);
}

uint32_t intc_pending(void)
{
    return reg_rd(INTC_BASE, INTC_IPR);
}

void intc_ack(uint32_t mask)
{
    reg_wr(INTC_BASE, INTC_IAR, mask);
}
