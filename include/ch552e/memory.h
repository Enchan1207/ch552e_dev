#ifndef CH552E_MEMORY_H
#define CH552E_MEMORY_H

#include <stddef.h>

#include "sdcc/identifier.h"

/**
 * @brief code領域からxRAM領域へデータをコピーする
 *
 * @param dest
 * @param src
 * @param length
 */
void memcpy_code_to_xdata(__xdata void* dest, const __code void* src, size_t length);

#endif /* CH552E_MEMORY_H */
