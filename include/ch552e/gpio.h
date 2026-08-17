#ifndef CH552E_GPIO_H
#define CH552E_GPIO_H

#include <sdcc/identifier.h>

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

#endif /* CH552E_GPIO_H */
