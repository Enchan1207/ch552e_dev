#include <ch552e/io.h>
#include <ch552e/memory.h>

#include "endpoint_0_private.h"
#include "hardware/usb/usb.h"

void usb_ep0_handle_packet(usb_ctx_t* ctx, uint8_t token, uint8_t length) {
    if (token == UIS_TOKEN_SETUP) {
        if (length != 8) {
            usb_ep0_stall();
            return;
        }

        usb_setup_packet_t __xdata* packet = (usb_setup_packet_t __xdata*)ep0_buffer;
        usb_setup_fifo_push_isr(packet);

        int8_t handle_result = usb_ep0_handle_setup(ctx, packet);

        if (handle_result < 0) {
            usb_ep0_stall();
            return;
        }

        return;
    }

    if (token == UIS_TOKEN_IN) {
        usb_ep0_handle_in(ctx);
    }

    if (token == UIS_TOKEN_OUT) {
        usb_ep0_handle_out(ctx, length);
    }
}
