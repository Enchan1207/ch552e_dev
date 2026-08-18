#include "internal.h"

static usb_ep0_ctx_t ctx_internal = {
    .state = USB_STATE_INIT,
};

usb_ep0_ctx_t* usb_ep0_ctx = &ctx_internal;

void usb_ep0_init(void) {
    UEP0_DMA = USB_EP0_DMA_ADDRESS;
    usb_ep0_reset();
}

void usb_ep0_reset(void) {
    ctx_internal.state = USB_STATE_INIT;

    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
    USB_DEV_AD = 0x00;
}
