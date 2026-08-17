#include <ch552e/io.h>
#include <ch552e/memory.h>

#include "hardware/usb/usb.h"
#include "usb_private.h"

__xdata __at(USB_EP0_DMA_ADDRESS)
uint8_t ep0_buffer[64];

/** EP0をSTALL状態にする */
static inline void usb_ep0_stall(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
}

/**
 * @brief EP0のSETUPパケットを処理する
 *
 * @param packet パケット
 * @return int8_t 送信するデータ長 (負数はエラー)
 */
static int8_t usb_ep0_handle_setup(const usb_setup_packet_t __xdata* packet);

/**
 * @brief EP0のINパケットを処理する
 */
static void usb_ep0_handle_in(void);

/**
 * @brief EP0のOUTパケットを処理する
 */
static void usb_ep0_handle_out(uint8_t length);

void usb_ep0_handle_packet(uint8_t token, uint8_t length) {
    if (token == UIS_TOKEN_SETUP) {
        if (length != 8) {
            usb_ep0_stall();

            P1_4 = 1;
            return;
        }

        usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;
        usb_setup_fifo_push_isr(packet);

        int8_t handle_result = usb_ep0_handle_setup(packet);

        if (handle_result < 0) {
            usb_ep0_stall();

            P1_4 = 1;
            return;
        }

        UEP0_T_LEN = handle_result;
        UEP0_CTRL =
            bUEP_T_TOG |
            bUEP_R_TOG |
            UEP_R_RES_ACK |
            UEP_T_RES_ACK;

        P1_4 = 0;
        return;
    }

    if (token == UIS_TOKEN_IN) {
        usb_ep0_handle_in();
    }

    if (token == UIS_TOKEN_OUT) {
        usb_ep0_handle_out(length);
    }
}

static int8_t usb_ep0_handle_setup(const usb_setup_packet_t __xdata* packet) {
    // SET_ADDRESS
    if (packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_SET_ADDRESS &&
        packet->wIndex == 0 &&
        packet->wLength == 0 &&
        packet->wValue <= 127) {
        ctx.ep0_state = USB_EP0_STATE_ADDRESS_PENDING;
        ctx.device_address_candidate = packet->wValue;

        return 0x00;
    }

    // GET_DESCRIPTOR
    if (packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_GET_DESCRIPTOR &&
        (packet->wValue >> 8) == USB_DESCRIPTOR_TYPE_DEVICE &&
        packet->wIndex == 0 &&
        (packet->wValue & 0xFF) == 0x00) {
        ctx.latest_request_type = REQ_GET_DESCRIPTOR;

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

static void usb_ep0_handle_in(void) {
    // デバイスアドレスの確定・保持
    if (ctx.ep0_state == USB_EP0_STATE_ADDRESS_PENDING) {
        USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | ctx.device_address_candidate;

        ctx.ep0_state = USB_EP0_STATE_IDLE;
        ctx.device_address_candidate = 0x00;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        return;
    }

    if (ctx.latest_request_type == REQ_GET_DESCRIPTOR) {
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

static void usb_ep0_handle_out(uint8_t length) {
    if (ctx.latest_request_type == REQ_GET_DESCRIPTOR && length == 0) {
        ctx.latest_request_type = REQ_NONE;
        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    }
}
