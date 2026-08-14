#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "func.h"
#include "hardware/uart1.h"
#include "hardware/usb.h"

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    uart_begin();

    usb_init();

    irq_enable();

    uart_print("Initialized.\r\n");

    while (1) {
        if (usb_debug_interrupt_flags != 0) {
            uint8_t buf = usb_debug_interrupt_flags;

            uart_print_bin(buf);
            uart_print("\r\n");

            if (buf & 0b10) {
                uart_print("UIF_TRANSFER\r\n");
                uart_print_bin(usb_debug_interrupt_status);
                uart_print("\r\n");
                uart_print_dec(usb_debug_interrupt_rx_length);
                uart_print("\r\n");
            }

            usb_debug_interrupt_flags = 0x00;
        }
    }
}
