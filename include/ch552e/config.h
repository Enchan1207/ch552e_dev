#ifndef CH552E_CONFIG_H
#define CH552E_CONFIG_H

#include "sdcc/identifier.h"

/**
 * @brief
 *      Safe mode control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Some protected SFRs, such as `CLOCK_CFG`, can only be written
 *      while the device is in safe mode.
 *
 *      To enter safe mode, write `0x55` followed immediately by `0xAA`:
 *
 *      @code
 *      SAFE_MOD = 0x55;
 *      SAFE_MOD = 0xAA;
 *      @endcode
 *
 *      Safe mode remains active for approximately 13 to 23 system clock
 *      cycles. Protected SFR writes must be completed within this period.
 *      Safe mode is automatically terminated when the period expires.
 *
 *      Writing any other value to this register terminates safe mode
 *      immediately.
 *
 * @note
 *      This register is write-only. Reading address `0xA1` accesses
 *      `CHIP_ID` instead of `SAFE_MOD`.
 *
 * @warning
 *      Interrupts may consume the limited safe-mode period. When multiple
 *      instructions are required between entering safe mode and writing a
 *      protected SFR, execute the sequence atomically or with interrupts
 *      temporarily disabled.
 */
__sfr __at(0xA1) SAFE_MOD;

/**
 * @brief
 *      System clock configuration register
 *      (Reset value: `0x83`)
 * @warning This register only can be written in safe mode.
 */
__sfr __at(0xB9) CLOCK_CFG;

#endif /* CH552E_CONFIG_H */
