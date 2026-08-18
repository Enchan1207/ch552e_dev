#include "internal.h"

void usb_ep0_handle_in(usb_ep0_ctx_t* ctx) {
    switch (ctx->state) {
        case USB_STATE_WAIT_DEVICE_ADDRESS:
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | ctx->address_pending.address_candidate;
            ctx->state = USB_STATE_IDLE;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;

        case USB_STATE_SEND_DEVICE_DESCRIPTOR:
            ctx->state = USB_STATE_WAIT_STATUS_OUT;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            return;

        default:
            break;
    }
}
