#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/uart1.h"

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    uart_begin();

    irq_enable();

    while (1) {
    }
}
