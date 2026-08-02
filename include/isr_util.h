#ifndef ISR_UTIL_H
#define ISR_UTIL_H

#include "hardware/intellisense_mock.h"

/**
 * @brief ISR definition
 * @note 引数にはベクタ番号を指定します。
 */
#define ISR(vect) void __isr_##vect(void) __interrupt(vect)

#endif /* ISR_UTIL_H */
