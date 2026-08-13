#include "hardware/uart1.h"

#include "ch552e/io.h"

struct uart1_ctx_t {
    volatile bool txBusy;
    const uint8_t* volatile txDataPtr;
    volatile size_t txRemaining;

    volatile uint8_t rxBuffer[UART1_RX_BUFSIZE];
    volatile uint8_t rxHead;
    volatile uint8_t rxTail;
};

static uart1_ctx_t uart1_internal = {
    .txBusy = false,
    .txDataPtr = NULL,
    .txRemaining = 0,

    .rxBuffer = {0},
    .rxHead = 0,
    .rxTail = 0,
};

uart1_ctx_t* uart1 = &uart1_internal;

// UART1割込み
ISR(INT_NO_UART1) {
    // 受信完了
    if (U1RI) {
        uint8_t data = SBUF1;
        uint8_t nextTail = (uart1->rxTail + 1) & (UART1_RX_BUFSIZE - 1);

        // バッファがいっぱいの場合は読み捨て
        if (nextTail != uart1->rxHead) {
            uart1->rxBuffer[uart1->rxTail] = data;
            uart1->rxTail = nextTail;
        }

        U1RI = 0;
    }

    // 送信完了
    if (U1TI) {
        U1TI = 0;

        if (uart1->txRemaining > 0) {
            SBUF1 = *uart1->txDataPtr;
            uart1->txDataPtr++;
            uart1->txRemaining--;
        } else {
            uart1->txBusy = false;
        }
    }
}

void uart_begin(void) {
    irq_disable();

    // TODO: UART1_BAUD_RATEから計算する
    SBAUD1 = 217;

    // 8/N/1, 倍速, 受信有効, 受信割込み有効
    U1SM0 = 0;
    U1SMOD = 1;
    U1REN = 1;

    IE_UART1 = 1;
}

void uart_write(uart1_ctx_t* ctx, const uint8_t* const data, size_t len) {
    uart_write_noblock(ctx, data, len);
    while (ctx->txBusy);
}

void uart_write_noblock(uart1_ctx_t* ctx, const uint8_t* const data, size_t len) {
    while (ctx->txBusy);

    if (len == 0) {
        return;
    }

    ctx->txDataPtr = data;
    ctx->txRemaining = len - 1;
    ctx->txBusy = true;

    // 最初の1byteを書いておく
    SBUF1 = *ctx->txDataPtr;
    ctx->txDataPtr++;
}

void uart_print(uart1_ctx_t* ctx, const char* const str) {
    uart_print_noblock(ctx, str);

    while (ctx->txBusy);
}

void uart_print_noblock(uart1_ctx_t* ctx, const char* const str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    uart_write_noblock(ctx, (const uint8_t*)str, len);
}

uint8_t uart_read(uart1_ctx_t* ctx) {
    while (ctx->rxHead == ctx->rxTail);

    uint8_t data = ctx->rxBuffer[ctx->rxHead];
    ctx->rxHead = (ctx->rxHead + 1) & (UART1_RX_BUFSIZE - 1);

    return data;
}

bool uart_available(uart1_ctx_t* ctx) {
    return ctx->rxHead != ctx->rxTail;
}
