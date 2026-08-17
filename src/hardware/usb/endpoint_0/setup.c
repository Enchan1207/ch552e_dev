#include "endpoint_0_private.h"

// MARK: - SET_ADDRESS

static inline int8_t usb_ep0_handle_set_address(usb_ctx_t* ctx, const usb_setup_packet_t __xdata* packet) {
    ctx->ep0_state = USB_EP0_STATE_ADDRESS_PENDING;
    ctx->device_address_candidate = packet->wValue;

    UEP0_T_LEN = 0x00;
    UEP0_CTRL =
        bUEP_T_TOG |
        UEP_R_RES_ACK |
        UEP_T_RES_ACK;

    return 0;
}

// MARK: - GET_DESCRIPTOR

static inline int8_t usb_ep0_handle_get_descriptor(usb_ctx_t* ctx, const usb_setup_packet_t __xdata* packet) {
    uint8_t descriptor_type = packet->wValue >> 8;

    const __code uint8_t* data;
    size_t length;

    switch (descriptor_type) {
        case USB_DESCRIPTOR_TYPE_DEVICE:
            const __code usb_device_descriptor_t* descriptor = usb_get_device_descriptor();

            data = descriptor;
            length = sizeof(*descriptor);
            break;

        case USB_DESCRIPTOR_TYPE_CONFIGURATION:
            // TODO: implement
            return -1;

        default:
            return -1;
    }

    // wLength以上は送らない
    length = length > packet->wLength ? packet->wLength : length;

    // データ送信を準備
    ctx->ep0_state = USB_EP0_STATE_DATA_IN;
    ctx->tx_data = data;
    ctx->tx_length = length;
    ctx->tx_offset = 0;

    usb_ep0_prepare_next_in_packet(ctx);

    return 0;
}

// MARK: - handler

int8_t usb_ep0_handle_setup(usb_ctx_t* ctx, const usb_setup_packet_t __xdata* packet) {
    // SET_ADDRESS
    if (packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_SET_ADDRESS &&
        packet->wIndex == 0 &&
        packet->wLength == 0 &&
        packet->wValue <= 127) {
        return usb_ep0_handle_set_address(ctx, packet);
    }

    // GET_DESCRIPTOR
    if (packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
        packet->bRequest == REQ_GET_DESCRIPTOR &&
        packet->wIndex == 0 &&
        (packet->wValue & 0xFF) == 0x00) {
        return usb_ep0_handle_get_descriptor(ctx, packet);
    }

    return -1;
}
