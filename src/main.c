#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "bit_util.h"

volatile bool isDataReceived = false;
volatile uint8_t uartBuffer = 0x00;

// UART1の割込みは送受信を区別しないので、ISR内で分岐する
ISR(INT_NO_UART1) {
    if (U1RI) {
        uartBuffer = SBUF1;
        isDataReceived = true;
        U1RI = 0;
    }

    if (U1TI) {
        U1TI = 0;
    }
}

void uart1_write(uint8_t data) {
    IE_UART1 = 0;
    U1TI = 0;
    SBUF1 = data;

    while (!U1TI) {
    }

    U1TI = 0;
    IE_UART1 = 1;
}

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    // P1.7 (Tx) をpush-pull出力にする
    reset(P1_MOD_OC, 7);
    set(P1_DIR_PU, 7);

    // P1.6 (Rx) を入力にする
    reset(P1_MOD_OC, 6);
    reset(P1_DIR_PU, 6);

    // P1.4 (インジケータ) を出力にする
    P1_4 = 0;
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    // baud rate構成
    // Fsys = 6MHz かつUARTは倍速で動いているので SBAUD1 = 256 - Fsys / 16 / baud
    // 今回は 9600 baud になるよう設定
    SBAUD1 = 217;

    // 8/N/1, 倍速, 受信有効, 受信割込み有効
    U1SM0 = 0;
    U1SMOD = 1;
    U1REN = 1;
    IE_UART1 = 1;

    // グローバル割込み有効化
    EA = 1;

    while (1) {
        if (!isDataReceived) {
            continue;
        }

        isDataReceived = false;
        uart1_write(uartBuffer);
    }
}
