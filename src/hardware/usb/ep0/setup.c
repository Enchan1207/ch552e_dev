#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stddef.h>

#include "../request.h"
#include "internal.h"

static bool is_get_descriptor(usb_setup_packet_ptr packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_D2H | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_GET_DESCRIPTOR &&
           packet->wIndex == 0 &&
           packet->wValue.l == 0x00;
}

static bool is_get_device_descriptor(usb_setup_packet_ptr packet) {
    return is_get_descriptor(packet) && ((packet->wValue.h) == USB_DESCRIPTOR_TYPE_DEVICE);
}

static bool is_get_configuration_descriptor(usb_setup_packet_ptr packet) {
    return is_get_descriptor(packet) && ((packet->wValue.h) == USB_DESCRIPTOR_TYPE_CONFIGURATION);
}

static bool is_set_address(usb_setup_packet_ptr packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_SET_ADDRESS &&
           packet->wIndex == 0 &&
           packet->wLength.raw == 0 &&
           packet->wValue.raw <= 127;
}

static bool is_set_configuration(usb_setup_packet_ptr packet) {
    return packet->bmRequestType == (MREQ_DIRECTION_H2D | MREQ_TYPE_STANDARD | MREQ_TARGET_DEVICE) &&
           packet->bRequest == REQ_SET_CONFIGURATION &&
           packet->wIndex == 0 &&
           packet->wLength.raw == 0 &&
           (packet->wValue.raw == 0 || packet->wValue.raw == 1);
}

bool usb_ep0_handle_setup(usb_setup_packet_ptr packet) {
    if (is_set_address(packet)) {
        usb_ep0_ctx->state = USB_STATE_WAIT_DEVICE_ADDRESS;
        usb_ep0_ctx->address_pending.address_candidate = packet->wValue.l;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_get_device_descriptor(packet)) {
        usb_ep0_ctx->state = USB_STATE_SEND_DEVICE_DESCRIPTOR;

        usb_device_descriptor_ptr descriptor = usb_get_device_descriptor();
        uint8_t descriptor_size = descriptor->bLength;

        memcpy_code_to_xdata(ep0_buffer, descriptor, descriptor_size);

        UEP0_T_LEN = descriptor_size > packet->wLength.raw ? packet->wLength.raw : descriptor_size;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_get_configuration_descriptor(packet)) {
        usb_ep0_ctx->state = USB_STATE_SEND_CONFIGURATION_DESCRIPTOR;

        usb_configuration_descriptor_ptr config = usb_get_configuration_descriptor();

        usb_ep0_ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_CONFIGURATION;
        usb_ep0_ctx->configuration_stream.remaining = config->wTotalLength;
        usb_ep0_ctx->configuration_stream.descriptor = config;
        usb_ep0_ctx->configuration_stream.descriptor_size = config->bLength;
        usb_ep0_ctx->configuration_stream.offset = 0;
        usb_ep0_ctx->configuration_stream.if_index = 0;
        usb_ep0_ctx->configuration_stream.index = 0;

        uint8_t filled_length = usb_ep0_prepare_descriptor();
        size_t tx_length = filled_length > packet->wLength.raw ? packet->wLength.raw : filled_length;

        UEP0_T_LEN = tx_length;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    if (is_set_configuration(packet)) {
        usb_ep0_ctx->state = USB_STATE_WAIT_SET_CONFIGURATION;
        usb_ep0_ctx->configuration_pending.configuration_candidate = packet->wValue.l;

        UEP0_T_LEN = 0;
        UEP0_CTRL = bUEP_T_TOG | bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
        return true;
    }

    return false;
}
