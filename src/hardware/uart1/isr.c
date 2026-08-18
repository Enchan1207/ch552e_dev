#include <ch552e/io.h>
#include <hardware/uart1.h>

#include "context.h"

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
