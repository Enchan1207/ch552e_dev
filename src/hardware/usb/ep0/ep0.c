#include "ep0.h"

#include <stddef.h>

#include "internal.h"
#include "usb_setup_fifo.h"

static __idata usb_ep0_ctx_t ctx_internal = {
    .state = USB_STATE_INIT,
};

__idata usb_ep0_ctx_t* usb_ep0_ctx = &ctx_internal;

__xdata __at(USB_EP0_BUFFER_ADDRESS)
uint8_t ep0_buffer[64];

void usb_ep0_init(void) {
    UEP0_DMA = USB_EP0_BUFFER_ADDRESS;
    usb_ep0_reset();
}

void usb_ep0_reset(void) {
    usb_ep0_ctx->state = USB_STATE_INIT;
    usb_ep0_ctx->configuration = 0x00;

    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
    USB_DEV_AD = 0x00;
}

static void copy_setup_packet(usb_setup_packet_t* _dest, const __xdata void* _src) {
    uint8_t* dest = _dest;
    const __xdata uint8_t* src = _src;

    for (size_t i = 0; i < sizeof(usb_setup_packet_t); i++) {
        *dest = *src;
        dest++;
        src++;
    }
}

void usb_ep0_handle_packet(uint8_t token, uint8_t length) {
    switch (token) {
        case UIS_TOKEN_SETUP:
            if (length != 8) {
                usb_ep0_stall();
                break;
            }

            usb_setup_packet_t __idata packet;
            copy_setup_packet(&packet, ep0_buffer);

            usb_setup_fifo_push_isr(&packet);

            bool result = usb_ep0_handle_setup(&packet);

            if (!result) {
                usb_ep0_stall();
                break;
            }

            break;

        case UIS_TOKEN_IN:
            usb_ep0_handle_in();
            break;

        case UIS_TOKEN_OUT:
            usb_ep0_handle_out(length);
            break;

        default:
            break;
    }
}
