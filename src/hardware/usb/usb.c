#include "hardware/usb.h"

#include <ch552e/io.h>
#include <stdint.h>

#include "usb_private.h"

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

__xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

volatile uint8_t usb_debug_interrupt_flags;
volatile uint8_t usb_debug_interrupt_status;
volatile uint8_t usb_debug_interrupt_rx_length;

static inline void usb_handle_bus_reset(void);
static inline void usb_handle_transfer(uint8_t status, uint8_t length);

// TODO　ステートマシンに起こす
uint8_t device_address_candidate = 0x00;
usb_request_type_t latest_request_type = REQ_NONE;

ISR(INT_NO_USB) {
    usb_debug_interrupt_flags = USB_INT_FG;

    if (UIF_TRANSFER) {
        uint8_t status = USB_INT_ST;
        uint8_t length = USB_RX_LEN;

        usb_debug_interrupt_status = status;
        usb_debug_interrupt_rx_length = length;

        usb_handle_transfer(status, length);

        UIF_TRANSFER = 0;
    }

    if (UIF_BUS_RST) {
        usb_handle_bus_reset();

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

static inline void usb_handle_bus_reset(void) {
    // TODO コンテキストもリセットする
    USB_DEV_AD = 0x00;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
}

static inline void usb_handle_transfer(uint8_t status, uint8_t length) {
    uint8_t endpoint = status & MASK_UIS_ENDP;
    uint8_t token = status & MASK_UIS_TOKEN;

    if (endpoint == 0 && token == UIS_TOKEN_SETUP) {
        if (length != 8) {
            // malformed SETUP packet
            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
            return;
        }

        usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;

        latest_request_type = packet->bRequest;

        // SET_ADDRESS
        if (packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
            packet->bRequest == REQ_SET_ADDRESS &&
            packet->wIndex == 0 &&
            packet->wLength == 0 &&
            packet->wValue <= 127) {
            if ((USB_DEV_AD & MASK_USB_ADDR) != 0) {
                // unexpected SET_ADDRESS
                UEP0_T_LEN = 0x00;
                UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
                return;
            }

            device_address_candidate = packet->wValue;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_NAK | UEP_T_RES_ACK | bUEP_T_TOG;
            return;
        }
    }

    if (endpoint == 0 && token == UIS_TOKEN_IN) {
        if (latest_request_type == REQ_SET_ADDRESS) {
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | device_address_candidate;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        }
    }
}
