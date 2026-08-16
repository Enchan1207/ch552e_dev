#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "func.h"
#include "hardware/uart1.h"
#include "hardware/usb.h"
#include "hardware/usb_setup_fifo.h"

int main(void) {
    usb_setup_packet_t setup_packet;

    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    // P1.4 (インジケータ) を出力にする
    P1_4 = 0;
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    uart_begin();

    usb_init();

    irq_enable();

    uart_print("Initialized.\r\n");

    while (1) {
        while (usb_setup_fifo_pop(&setup_packet)) {
            uart_print("SETUP bmRequestType:0x");
            uart_print_hex(setup_packet.bmRequestType);
            uart_print(" bRequest:0x");
            uart_print_hex(setup_packet.bRequest);
            uart_print(" wValue:0x");
            uart_print_hex((uint8_t)(setup_packet.wValue >> 8));
            uart_print_hex((uint8_t)setup_packet.wValue);
            uart_print(" wIndex:0x");
            uart_print_hex((uint8_t)(setup_packet.wIndex >> 8));
            uart_print_hex((uint8_t)setup_packet.wIndex);
            uart_print(" wLength:0x");
            uart_print_hex((uint8_t)(setup_packet.wLength >> 8));
            uart_print_hex((uint8_t)setup_packet.wLength);
            uart_print("\r\n");
        }

        if (usb_debug_interrupt_flags != 0) {
            uint8_t buf = usb_debug_interrupt_flags;

            uart_print_bin(buf);
            uart_print("\r\n");

            if (buf & 0b10) {
                uart_print("UIF_TRANSFER. INT_ST:");
                uart_print_bin(usb_debug_interrupt_status);
                uart_print(" DATA:");
                for (size_t i = 0; i < usb_debug_interrupt_rx_length; i++) {
                    uart_print_hex(*(usb_debug_ep0_buffer_ptr + i));
                }
                uart_print(" EP0_CTRL:");
                uart_print_hex(usb_debug_ep0_ctrl);
                uart_print(" EP0_T_LEN:");
                uart_print_dec(usb_debug_ep0_t_len);
                uart_print("\r\n");
            }

            usb_debug_interrupt_flags = 0x00;
        }
    }
}
