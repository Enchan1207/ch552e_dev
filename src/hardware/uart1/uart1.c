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
    .txBusy = false,
    .txDataPtr = NULL,
    .txRemaining = 0,

    .rxBuffer = {0},
    .rxHead = 0,
    .rxTail = 0,
};

// UART1割込み
ISR(INT_NO_UART1) {
    // 受信完了
    if (U1RI) {
        uint8_t data = SBUF1;
        uint8_t nextTail = (uart1.rxTail + 1) & (UART1_RX_BUFSIZE - 1);

        // バッファがいっぱいの場合は読み捨て
        if (nextTail != uart1.rxHead) {
            uart1.rxBuffer[uart1.rxTail] = data;
            uart1.rxTail = nextTail;
        }

        U1RI = 0;
    }

    // 送信完了
    if (U1TI) {
        U1TI = 0;

        if (uart1.txRemaining > 0) {
            SBUF1 = *uart1.txDataPtr;
            uart1.txDataPtr++;
            uart1.txRemaining--;
        } else {
            uart1.txBusy = false;
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
    while (uart1.txBusy);
}

void uart_write_noblock(const uint8_t* const data, size_t len) {
    while (uart1.txBusy);

    if (len == 0) {
        return;
    }

    uart1.txDataPtr = data;
    uart1.txRemaining = len - 1;
    uart1.txBusy = true;

    // 最初の1byteを書いておく
    SBUF1 = *uart1.txDataPtr;
    uart1.txDataPtr++;
}

void uart_print(const char* const str) {
    uart_print_noblock(str);

    while (uart1.txBusy);
}

void uart_print_noblock(const char* const str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    uart_write_noblock((const uint8_t*)str, len);
}

uint8_t uart_read(void) {
    while (uart1.rxHead == uart1.rxTail);

    uint8_t data = uart1.rxBuffer[uart1.rxHead];
    uart1.rxHead = (uart1.rxHead + 1) & (UART1_RX_BUFSIZE - 1);

    return data;
}

bool uart_available(void) {
    return uart1.rxHead != uart1.rxTail;
}

#undef UART1_BAUD_VALUE
