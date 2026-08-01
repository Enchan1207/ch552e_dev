#include <stdint.h>

// P1 port direction control and pull-up enable register
__sfr __at(0x93) P1_DIR_PU;

// P1 port output mode register
__sfr __at(0x93) P1_MOD_OC;

// P1 port input and output register
__sfr __at(0x90) P1;

void delay() {
    uint32_t i = 0;
    while (--i) {
    }
}

int main(void) {
    // setup

    while (1) {
        P1 = 0b00010000;
        delay();
        P1 = 0b00000000;
        delay();
    }
}
