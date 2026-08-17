#include <ch552e/io.h>
#include <ch552e/memory.h>

#include "hardware/usb/usb.h"
#include "usb_private.h"

static inline void usb_handle_bus_reset(void) {
    usb_reset_context();
    USB_DEV_AD = 0x00;
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
}

static inline void usb_handle_transfer(uint8_t status, uint8_t length) {
    uint8_t endpoint = status & MASK_UIS_ENDP;
    uint8_t token = status & MASK_UIS_TOKEN;

    if (endpoint == 0) {
        usb_ep0_handle_packet(token, length);
    }
}

static inline void usb_handle_suspend(void) {
    // NOTE 現時点では特に何もしない
}

ISR(INT_NO_USB) {
    if (UIF_TRANSFER) {
        usb_handle_transfer(USB_INT_ST, USB_RX_LEN);
        UIF_TRANSFER = 0;
    }

    if (UIF_BUS_RST) {
        usb_handle_bus_reset();
        UIF_BUS_RST = 0;
    }

    if (UIF_SUSPEND) {
        usb_handle_suspend();
        UIF_SUSPEND = 0;
    }
}
