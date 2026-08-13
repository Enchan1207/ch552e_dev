#include "func.h"

#include <ch552e/io.h>

#include "hardware/uart1.h"

void uart_print_dec(uint8_t value) {
    char buf[4] = {0};

    if (value >= 100) {
        buf[0] = '0' + (value / 100);
        buf[1] = '0' + ((value / 10) % 10);
        buf[2] = '0' + (value % 10);
    } else if (value >= 10) {
        buf[0] = '0' + (value / 10);
        buf[1] = '0' + (value % 10);
    } else {
        buf[0] = '0' + (value % 10);
    }

    uart_print(buf);
}

void uart_print_bin(uint8_t value) {
    char buf[9] = {0};
    for (size_t i = 0; i < 8; i++) {
        buf[i] = (value & (1 << (7 - i))) ? '1' : '0';
    }
    uart_print(buf);
}

static const char* const hex_literal = "0123456789ABCDEF";

void uart_print_hex(uint8_t value) {
    char buf[3] = {0};
    buf[0] = hex_literal[value >> 4];
    buf[1] = hex_literal[value & 0b1111];
    uart_print(buf);
}
