#include "endpoint_0_private.h"

void usb_ep0_handle_out(usb_ctx_t* ctx, uint8_t length) {
    if (ctx->ep0_state == USB_EP0_STATE_STATUS_OUT) {
        if (length != 0) {
            usb_ep0_stall();
            return;
        }

        ctx->ep0_state = USB_EP0_STATE_IDLE;
        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        return;
    }

    // NOTE: DATA_OUTを実装する場合はここに追加してゆく
    usb_ep0_stall();
}
