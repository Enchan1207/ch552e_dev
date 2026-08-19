#include <ch552e/memory.h>
#include <stdint.h>

void memcpy_code_to_xdata(__xdata void* _dest, const __code void* _src, size_t length) {
    __xdata uint8_t* dest = _dest;
    const __code uint8_t* src = _src;

    for (size_t i = 0; i < length; i++) {
        *dest = *src;
        dest++;
        src++;
    }
}