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

uart1_ctx_t uart1 = {
    .tx_busy = false,
    .tx_data_ptr = NULL,
    .tx_remaining = 0,
    .rx_head = 0,
    .rx_tail = 0,
};

__xdata __at(UART1_BUFFER_ADDRESS)
uint8_t rx_buffer[UART1_RX_BUFSIZE];

// UART1割込み
ISR(INT_NO_UART1) {
    // 受信完了
    if (U1RI) {
        uint8_t data = SBUF1;
        uint8_t nextTail = (uart1.rx_tail + 1) & (UART1_RX_BUFSIZE - 1);

        // バッファがいっぱいの場合は読み捨て
        if (nextTail != uart1.rx_head) {
            rx_buffer[uart1.rx_tail] = data;
            uart1.rx_tail = nextTail;
        }

        U1RI = 0;
    }

    // 送信完了
    if (U1TI) {
        U1TI = 0;

        if (uart1.tx_remaining > 0) {
            SBUF1 = *uart1.tx_data_ptr;
            uart1.tx_data_ptr++;
            uart1.tx_remaining--;
        } else {
            uart1.tx_busy = false;
        }
    }
}

void uart_begin(void) {
    irq_disable();

    SBAUD1 = UART1_BAUD_VALUE(UART1_BAUD_RATE);

    // 8/N/1, 倍速, 受信有効, 受信割込み有効
    U1SM0 = 0;
    U1SMOD = 1;
    U1REN = 1;

    IE_UART1 = 1;
}

void uart_write(const uint8_t* const data, size_t len) {
    uart_write_noblock(data, len);
    while (uart1.tx_busy);
}

void uart_write_noblock(const uint8_t* const data, size_t len) {
    while (uart1.tx_busy);

    if (len == 0) {
        return;
    }

    uart1.tx_data_ptr = data;
    uart1.tx_remaining = len - 1;
    uart1.tx_busy = true;

    // 最初の1byteを書いておく
    SBUF1 = *uart1.tx_data_ptr;
    uart1.tx_data_ptr++;
}

void uart_print(const char* const str) {
    uart_print_noblock(str);

    while (uart1.tx_busy);
}

void uart_print_noblock(const char* const str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    uart_write_noblock((const uint8_t*)str, len);
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
