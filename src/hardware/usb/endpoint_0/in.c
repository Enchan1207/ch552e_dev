#include "endpoint_0_private.h"

void usb_ep0_handle_in(usb_ctx_t* ctx) {
    switch (ctx->ep0_state) {
        // デバイスアドレスの確定・保持
        case USB_EP0_STATE_ADDRESS_PENDING:
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | ctx->device_address_candidate;

            ctx->ep0_state = USB_EP0_STATE_IDLE;
            ctx->device_address_candidate = 0x00;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;

        // データ送信
        case USB_EP0_STATE_DATA_IN:
            usb_ep0_prepare_next_in_packet(ctx);
            break;

        default:
            break;
    }
}
