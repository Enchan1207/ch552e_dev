#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stddef.h>

#include "../request.h"
#include "internal.h"

static inline bool is_get_descriptor(usb_setup_packet_ptr_t packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_GET_DESCRIPTOR &&
           packet->wIndex == 0 &&
           (packet->wValue & 0xFF) == 0x00;
}

static inline bool is_get_device_descriptor(usb_setup_packet_ptr_t packet) {
    return is_get_descriptor(packet) && ((packet->wValue >> 8) == USB_DESCRIPTOR_TYPE_DEVICE);
}

static inline bool is_get_configuration_descriptor(usb_setup_packet_ptr_t packet) {
    return is_get_descriptor(packet) && ((packet->wValue >> 8) == USB_DESCRIPTOR_TYPE_CONFIGURATION);
}

static inline bool is_set_address(usb_setup_packet_ptr_t packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_SET_ADDRESS &&
           packet->wIndex == 0 &&
           packet->wLength == 0 &&
           packet->wValue <= 127;
}

bool usb_ep0_handle_setup(usb_ep0_ctx_t* ctx, usb_setup_packet_ptr_t packet) {
    if (is_set_address(packet)) {
        ctx->state = USB_STATE_WAIT_DEVICE_ADDRESS;
        ctx->address_pending.address_candidate = packet->wValue;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_get_device_descriptor(packet)) {
        ctx->state = USB_STATE_GET_DEVICE_DESCRIPTOR;

        size_t descriptor_size = sizeof(usb_device_descriptor_t);
        size_t tx_length = descriptor_size > packet->wLength ? packet->wLength : descriptor_size;

        const __code usb_device_descriptor_t* descriptor = usb_get_device_descriptor();
        memcpy_code_to_xdata(ep0_buffer, descriptor, descriptor_size);

        UEP0_T_LEN = tx_length;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    // TODO get_configuration_descriptor

    return false;
}
