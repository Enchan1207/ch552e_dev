#ifndef HARDWARE_UART1_CONTEXT_H
#define HARDWARE_UART1_CONTEXT_H

#include <ch552e/io.h>
#include <hardware/uart1.h>
#include <stddef.h>
#include <stdint.h>

#ifndef UART1_RX_BUFFER_ADDRESS
/** UART1受信バッファxRAMアドレス */
#define UART1_RX_BUFFER_ADDRESS 0x0100
#endif

#ifndef UART1_TX_BUFFER_ADDRESS
/** UART1送信バッファxRAMアドレス */
#define UART1_TX_BUFFER_ADDRESS 0x0120
#endif

/** UART1コンテキスト */
typedef struct {
    volatile uint8_t rx_head;
    volatile uint8_t rx_tail;

    volatile bool tx_busy;
    volatile uint8_t tx_head;
    volatile uint8_t tx_tail;
} uart1_ctx_t;

extern __xdata __at(UART1_RX_BUFFER_ADDRESS)
/** UART1受信バッファ (xRAM) */
uint8_t rx_buffer[UART1_RX_BUFSIZE];

extern __xdata __at(UART1_TX_BUFFER_ADDRESS)
/** UART1送信バッファ (xRAM) */
uint8_t tx_buffer[UART1_TX_BUFSIZE];

extern __idata uart1_ctx_t uart1;

#endif /* HARDWARE_UART1_CONTEXT_H */
