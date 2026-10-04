#include <ch552e/io.h>
#include <hardware/uart1.h>

#include "context.h"

// UART1割込み
ISR(INT_NO_UART1) {
    // 受信完了
    if (U1RI) {
        uint8_t data = SBUF1;
        uint8_t next_tail = (uart1.rx_tail + 1) & (UART1_RX_BUFSIZE - 1);

        // バッファがいっぱいの場合は読み捨て
        if (next_tail != uart1.rx_head) {
            rx_buffer[uart1.rx_tail] = data;
            uart1.rx_tail = next_tail;
        }

        U1RI = 0;
    }

    // 送信完了
    if (U1TI) {
        if (uart1.tx_head != uart1.tx_tail) {
            SBUF1 = tx_buffer[uart1.tx_tail];
            uart1.tx_tail = (uart1.tx_tail + 1) & (UART1_TX_BUFSIZE - 1);
        } else {
            uart1.tx_busy = false;
        }

        U1TI = 0;
    }
}
