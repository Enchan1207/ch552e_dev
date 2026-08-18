#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stddef.h>

#include "../request.h"
#include "internal.h"

static inline bool is_get_descriptor(usb_setup_packet_ptr packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_GET_DESCRIPTOR &&
           packet->wIndex == 0 &&
           (packet->wValue & 0xFF) == 0x00;
}

static inline bool is_get_device_descriptor(usb_setup_packet_ptr packet) {
    return is_get_descriptor(packet) && ((packet->wValue >> 8) == USB_DESCRIPTOR_TYPE_DEVICE);
}

static inline bool is_get_configuration_descriptor(usb_setup_packet_ptr packet) {
    return is_get_descriptor(packet) && ((packet->wValue >> 8) == USB_DESCRIPTOR_TYPE_CONFIGURATION);
}

static inline bool is_set_address(usb_setup_packet_ptr packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_SET_ADDRESS &&
           packet->wIndex == 0 &&
           packet->wLength == 0 &&
           packet->wValue <= 127;
}

bool usb_ep0_handle_setup(usb_ep0_ctx_ptr ctx, usb_setup_packet_ptr packet) {
    if (is_set_address(packet)) {
        ctx->state = USB_STATE_WAIT_DEVICE_ADDRESS;
        ctx->address_pending.address_candidate = packet->wValue;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_get_device_descriptor(packet)) {
        ctx->state = USB_STATE_SEND_DEVICE_DESCRIPTOR;

        usb_device_descriptor_ptr descriptor = usb_get_device_descriptor();

        uint8_t descriptor_size = descriptor->bLength;
        size_t tx_length = descriptor_size > packet->wLength ? packet->wLength : descriptor_size;

        memcpy_code_to_xdata(ep0_buffer, descriptor, descriptor_size);

        UEP0_T_LEN = tx_length;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_get_configuration_descriptor(packet)) {
        ctx->state = USB_STATE_SEND_CONFIGURATION_DESCRIPTOR;

        usb_configuration_descriptor_ptr config = usb_get_configuration_descriptor();

        ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_CONFIGURATION;
        ctx->configuration_stream.remaining = config->wTotalLength;
        ctx->configuration_stream.descriptor = config;
        ctx->configuration_stream.descriptor_size = config->bLength;
        ctx->configuration_stream.offset = 0;
        ctx->configuration_stream.if_index = 0;
        ctx->configuration_stream.index = 0;

        uint8_t filled_length = usb_ep0_prepare_descriptor(ctx);
        size_t tx_length = filled_length > packet->wLength ? packet->wLength : filled_length;

        UEP0_T_LEN = tx_length;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    return false;
}
