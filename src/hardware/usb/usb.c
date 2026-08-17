#include "hardware/usb.h"

#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <stdbool.h>
#include <stdint.h>

#include "hardware/usb_setup_fifo.h"
#include "usb_private.h"

__xdata __at(USB_EP0_DMA_ADDRESS)
uint8_t ep0_buffer[64];

static void usb_handle_bus_reset(void);
static void usb_handle_transfer(uint8_t status, uint8_t length);

/** EP0をSTALL状態にする */
static inline void usb_ep0_stall(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
}

// TODO　ステートマシンに起こす
uint8_t device_address_candidate = 0x00;
usb_request_type_t latest_request_type = REQ_NONE;

ISR(INT_NO_USB) {
    if (UIF_TRANSFER) {
        usb_handle_transfer(USB_INT_ST, USB_RX_LEN);
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

static void usb_handle_bus_reset(void) {
    // TODO コンテキストもリセットする
    USB_DEV_AD = 0x00;
    device_address_candidate = 0x00;
    latest_request_type = REQ_NONE;
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
}

static void usb_handle_transfer(uint8_t status, uint8_t length) {
    uint8_t endpoint = status & MASK_UIS_ENDP;
    uint8_t token = status & MASK_UIS_TOKEN;

    if (endpoint == 0 && token == UIS_TOKEN_SETUP) {
        if (length != 8) {
            usb_ep0_stall();

            P1_4 = 1;
            return;
        }

        usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;
        usb_setup_fifo_push_isr(packet);

        int8_t handle_result = usb_ep0_handle_setup(packet);

        if (handle_result > 0) {
            UEP0_T_LEN = handle_result;
            UEP0_CTRL =
                bUEP_T_TOG |
                bUEP_R_TOG |
                UEP_R_RES_ACK |
                UEP_T_RES_ACK;

            P1_4 = 0;
        } else {
            usb_ep0_stall();

            P1_4 = 1;
        }

        return;
    }

    if (endpoint == 0 && token == UIS_TOKEN_IN) {
        if (latest_request_type == REQ_SET_ADDRESS) {
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | device_address_candidate;

            device_address_candidate = 0x00;
            latest_request_type = REQ_NONE;
            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
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

static int8_t usb_ep0_handle_setup(const usb_setup_packet_t __xdata* packet) {
    // SET_ADDRESS
    if (packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_SET_ADDRESS &&
        packet->wIndex == 0 &&
        packet->wLength == 0 &&
        packet->wValue <= 127) {
        latest_request_type = REQ_SET_ADDRESS;
        device_address_candidate = packet->wValue;

        return 0x00;
    }

    // GET_DESCRIPTOR
    if (packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_GET_DESCRIPTOR &&
        (packet->wValue >> 8) == DESCRIPTOR_TYPE_DEVICE &&
        packet->wIndex == 0 &&
        (packet->wValue & 0xFF) == 0x00) {
        latest_request_type = REQ_GET_DESCRIPTOR;

        const __code usb_device_descriptor_t* device_descriptor = usb_get_device_descriptor();

        uint8_t descriptor_length = device_descriptor->bLength;
        uint8_t packet_length =
            descriptor_length > packet->wLength
                ? (uint8_t)packet->wLength
                : descriptor_length;

        memcpy_code_to_xdata(ep0_buffer, device_descriptor, packet_length);

        return packet_length;
    }

    return -1;
}
