#ifndef INTELLISENSE_SDCC_H
#define INTELLISENSE_SDCC_H

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

#endif

#endif /** INTELLISENSE_SDCC_H */
