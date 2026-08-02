#include <stdint.h>

#define SFR(addr, name) \
    __sfr __at(addr)    \
        name

#define SBIT(addr, name) \
    __sbit __at(addr)    \
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
SBIT(0x90, P1_0);
SBIT(0x91, P1_1);
SBIT(0x92, P1_2);
SBIT(0x93, P1_3);
SBIT(0x94, P1_4);
SBIT(0x95, P1_5);
SBIT(0x96, P1_6);
SBIT(0x97, P1_7);

// UART1 control register
SFR(0xC0, SCON1);
SBIT(0xC0, U1RI);
SBIT(0xC1, U1TI);
SBIT(0xC4, U1REN);
SBIT(0xC5, U1SMOD);
SBIT(0xC7, U1SM0);

// UART1 data register
SFR(0xC1, SBUF1);

// UART1 baud rate setting register
SFR(0xC2, SBAUD1);

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
    P1_4 = 0;
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    // baud rate構成
    // Fsys = 6MHz かつUARTは倍速で動いているので SBAUD1 = 256 - Fsys / 16 / baud
    // 今回は 9600 baud になるよう設定
    SBAUD1 = 217;

    // 8/N/1, 倍速, 受信有効
    U1SM0 = 0;
    U1SMOD = 1;
    U1REN = 1;

    while (1) {
        // 待機
        if (!U1RI) {
            continue;
        }
        P1_4 = 1;
        U1RI = 0;
        uint8_t data = SBUF1;

        // エコーバック
        U1TI = 0;
        SBUF1 = data;

        while (!U1TI) {
        }

        U1TI = 0;
        P1_4 = 0;
    }
}
