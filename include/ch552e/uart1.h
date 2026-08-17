#ifndef CH552E_UART1_H
#define CH552E_UART1_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      UART1 control register
 *      (Reset value: `0x40`)
 * @note This register is bit-accessible with `U1RI`, `U1TI`, `U1REN`, `U1SMOD`, and `U1SM0`.
 */
__sfr __at(0xC0) SCON1;

/**
 * @brief UART1 receive interrupt flag
 * @note Set by hardware after a data byte is received successfully.
 *       This flag must be cleared by software.
 */
__sbit __at(0xC0) U1RI;

/**
 * @brief UART1 transmit interrupt flag
 * @note Set by hardware after a data byte has been transmitted.
 *       This flag must be cleared by software.
 */
__sbit __at(0xC1) U1TI;

/**
 * @brief UART1 receive enable control
 * @note 0: receive disabled / 1: receive enabled
 */
__sbit __at(0xC4) U1REN;

/**
 * @brief UART1 baud-rate mode selection
 * @note 0: slow mode (`Fsys / 32`) / 1: fast mode (`Fsys / 16`)
 */
__sbit __at(0xC5) U1SMOD;

/**
 * @brief UART1 operating mode selection
 * @note 0: 8-bit asynchronous communication / 1: 9-bit asynchronous communication
 */
__sbit __at(0xC7) U1SM0;

/**
 * @brief UART1 transmit and receive data register
 * @note
 *      The transmit and receive registers are physically separate.
 *      Writing accesses the transmit register; reading accesses the receive register.
 */
__sfr __at(0xC1) SBUF1;

/**
 * @brief UART1 baud-rate setting register
 * @note
 *      `U1SMOD = 0`: `SBAUD1 = 256 - Fsys / 32 / baud`
 *
 *      `U1SMOD = 1`: `SBAUD1 = 256 - Fsys / 16 / baud`
 */
__sfr __at(0xC2) SBAUD1;

#endif /* CH552E_UART1_H */
