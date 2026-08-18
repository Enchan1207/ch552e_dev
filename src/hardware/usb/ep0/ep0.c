#include "ep0.h"

#include "internal.h"
#include "usb_setup_fifo.h"

static usb_ep0_ctx_t ctx = {
    .state = USB_STATE_INIT,
};

__xdata __at(USB_EP0_DMA_ADDRESS)
uint8_t ep0_buffer[64];

void usb_ep0_init(void) {
    UEP0_DMA = USB_EP0_DMA_ADDRESS;
    usb_ep0_reset();
}

void usb_ep0_reset(void) {
    ctx.state = USB_STATE_INIT;

    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
    USB_DEV_AD = 0x00;
}

void usb_ep0_handle_packet(uint8_t token, uint8_t length) {
    switch (token) {
        case UIS_TOKEN_SETUP:
            if (length != 8) {
                usb_ep0_stall();
                break;
            }

            usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;
            usb_setup_fifo_push_isr(packet);

            bool result = usb_ep0_handle_setup(&ctx, packet);

            if (!result) {
                usb_ep0_stall();
                break;
            }

            break;

        case UIS_TOKEN_IN:
            usb_ep0_handle_in(&ctx);
            break;

        case UIS_TOKEN_OUT:
            usb_ep0_handle_out(&ctx, length);
            break;

        default:
            break;
    }
}
