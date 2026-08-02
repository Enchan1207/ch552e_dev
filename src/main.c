#include <stdint.h>

#define SFR(addr, name) \
    __sfr __at(addr)    \
    name

// sage mode register
SFR(0xA1, SAFE_MOD);

// clock config resister
SFR(0xB9, CLOCK_CFG);

// P1 port direction control and pull-up enable register
SFR(0x93, P1_DIR_PU);

// P1 port output mode register
SFR(0x92, P1_MOD_OC);

// P1 port input and output register
SFR(0x90, P1);

// UART1 control register
SFR(0xC0, SCON1);

// UART1 data register
SFR(0xC1, SBUF1);

// UART1 baud rate setting register
SFR(0xC2, SBAUD1);

#define U1SM0_bp 7
#define U1RESERVED_bp 6
#define U1SMOD_bp 5
#define U1REN_bp 4
#define U1TB8_bp 3
#define U1RB8_bp 2
#define U1TI_bp 1
#define U1RI_bp 0

#define _BV(n) (1u << n)

#define set(addr, bitpos) addr |= _BV(bitpos);
#define get(addr, bitpos) (addr & _BV(bitpos))
#define reset(addr, bitpos) addr &= ~_BV(bitpos);

void delay(void) {
    volatile uint16_t i = 0;
    while (--i) {
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

    // P1.4 (インジケータ) を出力にする
    reset(P1, 4);
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    // baud rate構成
    // Fsys = 6MHz かつUARTは倍速で動いているので SBAUD1 = 256 - Fsys / 16 / baud
    // 今回は 9600 baud になるよう設定
    SBAUD1 = 217;

    // 8/N/1, 倍速, 受信有効
    reset(SCON1, U1SM0_bp);
    set(SCON1, U1SMOD_bp);
    set(SCON1, U1REN_bp);

    while (1) {
        // 待機
        if (!get(SCON1, U1RI_bp)) {
            continue;
        }
        set(P1, 4);
        reset(SCON1, U1RI_bp);
        uint8_t data = SBUF1;

        // エコーバック
        reset(SCON1, U1TI_bp);
        SBUF1 = data;

        while (!get(SCON1, U1TI_bp)) {
        }

        reset(SCON1, U1TI_bp);
        reset(P1, 4);
    }
}
