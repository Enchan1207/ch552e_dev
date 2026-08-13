#ifndef FUNC_H
#define FUNC_H

#include <stdint.h>

void print_usb_interrupt_flags(void);

void uart_print_dec(uint8_t value);
void uart_print_bin(uint8_t value);
void uart_print_hex(uint8_t value);

#endif /* FUNC_H */
