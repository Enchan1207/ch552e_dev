#ifndef HARDWARE_CH552E_H
#define HARDWARE_CH552E_H

#include "hardware/intellisense_mock.h"

/** safe mode register */
__sfr __at(0xA1) SAFE_MOD;

/**
 * @brief
 *      System clock configuration register
 *      (Reset value: `0x83`)
 * @warning This register only can be written in safe mode.
 */
__sfr __at(0xB9) CLOCK_CFG;

/**
 * @brief
 *      P1 port direction control and pull-up enable register
 *      (Reset value: `0xFF`)
 * @note
 *      Set direction control in push-pull output mode (0: in / 1: out)
 *
 *      Set pull-up register enable control in open-drain output mode (0: disabled / 1: enabled)
 */
__sfr __at(0x93) P1_DIR_PU;

/**
 * @brief
 *      P1 port output mode setting register
 *      (Reset value: `0xFF`)
 * @note 0: push-pull output / 1: open-drain output
 */
__sfr __at(0x92) P1_MOD_OC;

/**
 * @brief Port 1 Input/Output register
 * @note This register is bit-accessible with `P1_0` ~ `P1_7`.
 */
__sfr __at(0x90) P1;
__sbit __at(0x90) P1_0;
__sbit __at(0x91) P1_1;
__sbit __at(0x92) P1_2;
__sbit __at(0x93) P1_3;
__sbit __at(0x94) P1_4;
__sbit __at(0x95) P1_5;
__sbit __at(0x96) P1_6;
__sbit __at(0x97) P1_7;

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

#endif /* HARDWARE_CH552E_H */
