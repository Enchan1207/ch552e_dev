#ifndef INTELLISENSE_SDCC_H
#define INTELLISENSE_SDCC_H

/**
 * __sfr など、SDCC独自の識別子は VSCode intellisense では利用できないため、
 * マクロ __INTELLISENSE__ が定義されている場合はこれらを安全な定義に置き換える
 */

#ifdef __INTELLISENSE__

#define __sfr volatile unsigned char
#define __sbit volatile unsigned char
#define __at(address)

#define __data
#define __idata
#define __pdata
#define __xdata
#define __code const

#define __bit unsigned char
#define __critical
#define __reentrant
#define __using(x)

#define __interrupt(x)

#endif

#endif /** INTELLISENSE_SDCC_H */
