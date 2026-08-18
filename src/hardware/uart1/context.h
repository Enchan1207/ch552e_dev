#ifndef HARDWARE_UART1_CONTEXT_H
#define HARDWARE_UART1_CONTEXT_H

#include <ch552e/io.h>
#include <hardware/uart1.h>
#include <stddef.h>
#include <stdint.h>

/** UART1の受信バッファxRAMアドレス */
#define UART1_BUFFER_ADDRESS 0x0100

typedef struct {
    volatile bool tx_busy;
    const uint8_t* volatile tx_data_ptr;
    volatile size_t tx_remaining;
    volatile uint8_t rx_head;
    volatile uint8_t rx_tail;
} uart1_ctx_t;

extern __xdata __at(UART1_BUFFER_ADDRESS)
/** xRAM上のUART1のバッファ */
uint8_t rx_buffer[UART1_RX_BUFSIZE];

extern __idata uart1_ctx_t uart1;

#endif /* HARDWARE_UART1_CONTEXT_H */
