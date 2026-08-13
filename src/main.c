#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/uart1.h"

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

__xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

uint8_t usbIntflags = 0x00;

ISR(INT_NO_USB) {
    usbIntflags = USB_INT_FG;

    if (UIF_TRANSFER) {
        // TODO
        UIF_TRANSFER = 0;
    }

    if (UIF_BUS_RST) {
        // TODO
        UIF_BUS_RST = 0;
    }

    if (UIF_SUSPEND) {
        // TODO
        UIF_SUSPEND = 0;
    }
}

void print_hex(uint8_t value) {
    char buf[9] = "00000000";
    for (size_t i = 0; i < 8; i++) {
        buf[i] = (value & (1 << (7 - i))) ? '1' : '0';
    }

    uart_print(buf);
}

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    uart_begin();

    // USBエンドポイント0の初期化
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0;
    USB_DEV_AD = 0x00;
    UEP0_DMA = USB_EP0_DMA_ADDRESS;

    // USB割込み有効化
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;

    // Full-Speed USBポートを有効化
    UDEV_CTRL = bUD_PD_DIS | bUD_PORT_EN;

    // 内部D+プルアップを有効化, DMA有効化
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;

    irq_enable();

    uart_print("Initialized.\r\n");

    while (1) {
        if (usbIntflags != 0) {
            print_hex(usbIntflags);
            uart_print("\r\n");
            usbIntflags = 0x00;
        }
    }
}
