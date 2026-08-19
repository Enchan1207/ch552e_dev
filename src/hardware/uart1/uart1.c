#include <ch552e/io.h>
#include <hardware/uart1.h>

#include "context.h"

#ifndef F_SYS
#error F_SYS is not defined.
#endif

/** ボーレート計算マクロ */
#define UART1_BAUD_VALUE(baud)                                   \
    ((uint8_t)(256UL -                                           \
               (((uint32_t)(F_SYS) + (8UL * (uint32_t)(baud))) / \
                (16UL * (uint32_t)(baud)))))

__idata uart1_ctx_t uart1 = {
    .rx_head = 0,
    .rx_tail = 0,

    .tx_busy = false,
    .tx_head = 0,
    .tx_tail = 0,
};

__xdata __at(UART1_RX_BUFFER_ADDRESS)
uint8_t rx_buffer[UART1_RX_BUFSIZE];

__xdata __at(UART1_TX_BUFFER_ADDRESS)
uint8_t tx_buffer[UART1_TX_BUFSIZE];

void uart_begin(void) {
    SBAUD1 = UART1_BAUD_VALUE(UART1_BAUD_RATE);

    // 8/N/1, 倍速, 受信有効, 受信割込み有効
    U1SM0 = 0;
    U1SMOD = 1;
    U1REN = 1;

    IE_UART1 = 1;
}

void uart_write(uint8_t data) {
    uint8_t next_head = (uart1.tx_head + 1) & (UART1_TX_BUFSIZE - 1);

    while (next_head == uart1.tx_tail) {
        // TXバッファがいっぱい
    }

    tx_buffer[uart1.tx_head] = data;
    uart1.tx_head = next_head;

    if (!uart1.tx_busy) {
        uart1.tx_busy = true;

        SBUF1 = tx_buffer[uart1.tx_tail];
        uart1.tx_tail = (uart1.tx_tail + 1) & (UART1_TX_BUFSIZE - 1);
    }
}

void uart_print(const char* const str) {
    const char* p = str;

    while (*p != '\0') {
        uart_write(*p++);
    }
}

static const __code char hex_literal[] = "0123456789ABCDEF";

void uart_print_hex(uint8_t value) {
    uart_write(hex_literal[value >> 4]);
    uart_write(hex_literal[value & 0b1111]);
}

uint8_t uart_read(void) {
    while (uart1.rx_head == uart1.rx_tail);

    uint8_t data = rx_buffer[uart1.rx_head];
    uart1.rx_head = (uart1.rx_head + 1) & (UART1_RX_BUFSIZE - 1);

    return data;
}

bool uart_available(void) {
    return uart1.rx_head != uart1.rx_tail;
}

#undef UART1_BAUD_VALUE
