#include <stdint.h>

// P1 port direction control and pull-up enable register
__sfr __at(0x93) P1_DIR_PU;

// P1 port output mode register
__sfr __at(0x92) P1_MOD_OC;

// P1 port input and output register
__sfr __at(0x90) P1;

#define PORTMASK(n) (1u << n)
#define P1_IN(n) (P1 >> n & 0b1)

int main(void) {
    // P1.4 をpush-pull出力にする
    P1_MOD_OC &= ~PORTMASK(4);
    P1_DIR_PU |= PORTMASK(4);

    // P1.5を入力にする
    P1_MOD_OC &= ~PORTMASK(5);
    P1_DIR_PU &= ~PORTMASK(5);

    while (1) {
        if (P1_IN(5)) {
            P1 &= ~PORTMASK(4);
        } else {
            P1 |= PORTMASK(4);
        }
    }
}
