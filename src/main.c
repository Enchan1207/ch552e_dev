#include <stdbool.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/ch552e.h"
#include "isr_util.h"

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    // グローバル割込み有効化
    EA = 1;

    while (1) {
        // loop
    }
}
