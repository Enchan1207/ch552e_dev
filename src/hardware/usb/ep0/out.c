#include "internal.h"

void usb_ep0_handle_out(uint8_t length) {
    switch (ctx->state) {
        case USB_STATE_WAIT_STATUS_OUT:
            if (length != 0) {
                usb_ep0_stall();
                break;
            }

            ctx->state = USB_STATE_IDLE;
            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;

        default:
            break;
    }
}
