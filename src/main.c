#include <stdint.h>

// P1 port direction control and pull-up enable register
__sfr __at(0x93) P1_DIR_PU;

// P1 port output mode register
__sfr __at(0x92) P1_MOD_OC;

// P1 port input and output register
__sfr __at(0x90) P1;

#define P1_4_MASK (1u << 4)

void delay(void) {
    volatile uint16_t i = 0;
    while (--i) {
    }
}

int main(void) {
    // P1.4 をpush-pull出力にする
    P1_MOD_OC &= ~P1_4_MASK;
    P1_DIR_PU |= P1_4_MASK;

    while (1) {
        P1 |= P1_4_MASK;
        delay();

        P1 &= ~P1_4_MASK;
        delay();
    }
}
