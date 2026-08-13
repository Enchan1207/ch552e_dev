#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/uart1.h"

void delay(void) {
    volatile uint16_t counter = 0;
    while (--counter) {
    }
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

    uart_begin();

    // P1.4 (インジケータ) を出力にする
    P1_4 = 0;
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    P1_4 = 1;
    delay();
    P1_4 = 0;

    irq_enable();

    static const char __xdata message[] = "Hello from xdata!\r\n";

    while (1) {
        uart_print(message);
        delay();
    }
}
