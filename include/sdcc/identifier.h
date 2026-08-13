#ifndef INTELLISENSE_SDCC_H
#define INTELLISENSE_SDCC_H

#include <stdint.h>

/**
 * __sfr など、SDCC独自の識別子は VSCode intellisense では利用できないため、
 * マクロ __INTELLISENSE__ が定義されている場合はこれらを安全な定義に置き換える
 */

#ifdef __INTELLISENSE__

/** SFR (Special Function Register) 定義 */
#define __sfr volatile uint8_t

/** 16bit SFR 定義 */
#define __sfr16 volatile uint16_t

/**
 * @brief SFRビットアクセス
 * @details 8の倍数アドレスに割り当てられているレジスタにはビット単位でアクセスすることができます。
 */
#define __sbit volatile uint8_t

/** アドレス指定子 */
#define __at(address)

/** メモリ空間修飾子 (iRAM 下位128byte) */
#define __data

/** メモリ空間修飾子 (iRAM) */
#define __idata

/** メモリ空間修飾子 */
#define __pdata

/** メモリ空間修飾子 (xRAM) */
#define __xdata

/** メモリ空間修飾子 (プログラムメモリ) */
#define __code const

/** メモリ空間修飾子 (ビットアドレス可能領域, 0x20 - 0x2F) */
#define __bit uint8_t

#define __critical
#define __reentrant
#define __using(x)

/** ISR修飾子 */
#define __interrupt(x)

#endif

#endif /** INTELLISENSE_SDCC_H */
