#include "hardware/usb.h"

#include <ch552e/io.h>
#include <stdint.h>

#include "hardware/usb_setup_fifo.h"
#include "usb_private.h"

__xdata __at(USB_EP0_DMA_ADDRESS)
uint8_t ep0_buffer[64];

static inline void usb_handle_bus_reset(void);
static inline void usb_handle_transfer(uint8_t status, uint8_t length);
static inline void usb_arm_ep0_for_setup(void);

// TODO: 構造体にする
static const __code uint8_t device_descriptor_mock[] = {
    18,    // bLength
    0x01,  // bDescriptorType = DEVICE

    0x00, 0x02,  // bcdUSB = 2.00

    0x00,  // bDeviceClass
    0x00,  // bDeviceSubClass
    0x00,  // bDeviceProtocol

    64,  // bMaxPacketSize0

    0x34, 0x12,  // idVendor  example
    0x78, 0x56,  // idProduct example

    0x00, 0x01,  // bcdDevice

    0x00,  // iManufacturer
    0x00,  // iProduct
    0x00,  // iSerialNumber

    0x01  // bNumConfigurations
};

// TODO　ステートマシンに起こす
uint8_t device_address_candidate = 0x00;
usb_request_type_t latest_request_type = REQ_NONE;

ISR(INT_NO_USB) {
    if (UIF_TRANSFER) {
        uint8_t status = USB_INT_ST;
        uint8_t length = USB_RX_LEN;

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
    IE_USB = 0;
    usb_setup_fifo_init();

    USB_CTRL = 0x00;

    // USBエンドポイント0の初期化
    UEP0_DMA = USB_EP0_DMA_ADDRESS;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
    USB_DEV_AD = 0x00;

    // 内部プルダウン無効
    UDEV_CTRL = bUD_PD_DIS;

    USB_CTRL =
        bUC_DEV_PU_EN |  // 内部D+プルアップ機能を利用する
        bUC_INT_BUSY |   // UIF_TRANSFERがnegateされるまでR_NAK
        bUC_DMA_EN;      // USB DMAを利用する

    // ポート有効化
    UDEV_CTRL |= bUD_PORT_EN;

    // USB割込み初期化
    USB_INT_FG = 0xFF;
    USB_INT_EN =
        // bUIE_SUSPEND |
        // bUIE_DEV_NAK |
        bUIE_TRANSFER |  // 転送完了イベント
        bUIE_BUS_RST;    // バスリセットイベント

    IE_USB = 1;
}

static inline void usb_handle_bus_reset(void) {
    // TODO コンテキストもリセットする
    USB_DEV_AD = 0x00;
    device_address_candidate = 0x00;
    latest_request_type = REQ_NONE;
    usb_arm_ep0_for_setup();
}

static inline void usb_arm_ep0_for_setup(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
}

static inline void usb_handle_transfer(uint8_t status, uint8_t length) {
    uint8_t endpoint = status & MASK_UIS_ENDP;
    uint8_t token = status & MASK_UIS_TOKEN;

    if (endpoint == 0 && token == UIS_TOKEN_SETUP) {
        if (length != 8) {
            // malformed SETUP packet
            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;

            P1_4 = 1;
            return;
        }

        usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;
        usb_setup_fifo_push_isr(packet);

        // SET_ADDRESS
        if (packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
            packet->bRequest == REQ_SET_ADDRESS &&
            packet->wIndex == 0 &&
            packet->wLength == 0 &&
            packet->wValue <= 127) {
            latest_request_type = REQ_SET_ADDRESS;
            device_address_candidate = packet->wValue;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL =
                bUEP_T_TOG |
                bUEP_R_TOG |
                UEP_R_RES_ACK |
                UEP_T_RES_ACK;

            P1_4 = 0;
            return;
        }

        // GET_DESCRIPTOR
        if (packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
            packet->bRequest == REQ_GET_DESCRIPTOR &&
            (packet->wValue >> 8) == DESCRIPTOR_TYPE_DEVICE &&
            (packet->wValue & 0xFF) == 0x00) {
            latest_request_type = REQ_GET_DESCRIPTOR;

            uint8_t descriptor_length = sizeof device_descriptor_mock;
            uint8_t packet_length = descriptor_length > packet->wLength ? packet->wLength : descriptor_length;

            for (uint8_t i = 0; i < packet_length; i++) {
                ep0_buffer[i] = device_descriptor_mock[i];
            }

            UEP0_T_LEN = packet_length;

            UEP0_CTRL =
                bUEP_T_TOG |
                bUEP_R_TOG |
                UEP_R_RES_ACK |
                UEP_T_RES_ACK;

            P1_4 = 0;
            return;
        }

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;

        P1_4 = 1;
        return;
    }

    if (endpoint == 0 && token == UIS_TOKEN_IN) {
        if (latest_request_type == REQ_SET_ADDRESS) {
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | device_address_candidate;

            device_address_candidate = 0x00;
            latest_request_type = REQ_NONE;
            usb_arm_ep0_for_setup();
            return;
        }

        if (latest_request_type == REQ_GET_DESCRIPTOR) {
            /*
             * Data IN完了。
             * 次はStatus OUT (DATA1 ZLP) を待つ。
             */
            UEP0_T_LEN = 0x00;
            UEP0_CTRL =
                bUEP_R_TOG |
                UEP_R_RES_ACK |
                UEP_T_RES_NAK;
            return;
        }
    }

    if (endpoint == 0 && token == UIS_TOKEN_OUT) {
        if (latest_request_type == REQ_GET_DESCRIPTOR && length == 0) {
            latest_request_type = REQ_NONE;
            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        }
    }
}
