#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "func.h"
#include "hardware/uart1.h"

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

__xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

volatile uint8_t usb_interrupt_flags = 0x00;
volatile uint8_t usb_interrupt_status = 0x00;
volatile uint8_t usb_rx_length = 0x00;

ISR(INT_NO_USB) {
    usb_interrupt_flags = USB_INT_FG;

    if (UIF_TRANSFER) {
        usb_interrupt_status = USB_INT_ST;
        usb_rx_length = USB_RX_LEN;

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
        if (usb_interrupt_flags != 0) {
            uint8_t buf = usb_interrupt_flags;

            uart_print_bin(buf);
            uart_print("\r\n");

            if (buf & 0b10) {
                uart_print("UIF_TRANSFER\r\n");
                uart_print_bin(usb_interrupt_status);
                uart_print("\r\n");
                uart_print_dec(usb_rx_length);
                uart_print("\r\n");
            }

            usb_interrupt_flags = 0x00;
        }
    }
}
