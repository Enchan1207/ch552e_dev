#include "func.h"

#include <ch552e/io.h>

#include "hardware/uart1.h"

static const __code char hex_literal[] = "0123456789ABCDEF";

void uart_print_hex(uint8_t value) {
    uart_write_byte(hex_literal[value >> 4]);
    uart_write_byte(hex_literal[value & 0b1111]);
}
