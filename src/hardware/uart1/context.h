#ifndef HARDWARE_UART1_CONTEXT_H
#define HARDWARE_UART1_CONTEXT_H

#include <ch552e/io.h>
#include <hardware/uart1.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    volatile bool txBusy;
    const uint8_t* volatile txDataPtr;
    volatile size_t txRemaining;

    volatile uint8_t rxBuffer[UART1_RX_BUFSIZE];
    volatile uint8_t rxHead;
    volatile uint8_t rxTail;
} uart1_ctx_t;

extern uart1_ctx_t uart1;

#endif /* HARDWARE_UART1_CONTEXT_H */
