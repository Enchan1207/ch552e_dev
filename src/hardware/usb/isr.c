#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdint.h>

#include "ep0/internal.h"

ISR(INT_NO_USB) {
    if (UIF_TRANSFER) {
        uint8_t status = USB_INT_ST;
        uint8_t length = USB_RX_LEN;

        uint8_t endpoint = status & MASK_UIS_ENDP;
        uint8_t token = status & MASK_UIS_TOKEN;

        if (endpoint == 0) {
            usb_ep0_handle_packet(token, length);
        }

        // TODO: EP1-4のパケットを処理する

        UIF_TRANSFER = 0;
    }

    if (UIF_BUS_RST) {
        usb_ep0_reset();

        UIF_BUS_RST = 0;
    }

    if (UIF_SUSPEND) {
        // NOTE: 現時点では特に何もしない
        UIF_SUSPEND = 0;
    }
}
