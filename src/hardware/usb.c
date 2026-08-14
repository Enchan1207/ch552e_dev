#include "hardware/usb.h"

#include <ch552e/io.h>
#include <stdint.h>

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

__xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

volatile uint8_t usb_debug_interrupt_flags;
volatile uint8_t usb_debug_interrupt_status;
volatile uint8_t usb_debug_interrupt_rx_length;

ISR(INT_NO_USB) {
    usb_debug_interrupt_flags = USB_INT_FG;

    if (UIF_TRANSFER) {
        // TODO
        usb_debug_interrupt_status = USB_INT_ST;
        usb_debug_interrupt_rx_length = USB_RX_LEN;

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

void usb_init(void) {
    irq_disable();

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
}
