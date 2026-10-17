#include "internal.h"

void usb_ep0_handle_in(void) {
    switch (usb_ep0_ctx.state) {
        case USB_STATE_WAIT_DEVICE_ADDRESS:
            USB_DEV_AD = (USB_DEV_AD & ~MASK_USB_ADDR) | usb_ep0_ctx.address_pending.address_candidate;
            usb_ep0_ctx.state = USB_STATE_IDLE;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            break;

        case USB_STATE_SEND_DEVICE_DESCRIPTOR:
            usb_ep0_ctx.state = USB_STATE_WAIT_STATUS_OUT;

            UEP0_T_LEN = 0x00;
            UEP0_CTRL = bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            return;

        case USB_STATE_SEND_CONFIGURATION_DESCRIPTOR:
            // wLengthに達したら、最後のパケットが64バイトでもデータステージを終了する。
            if (usb_ep0_ctx.config_stream.request_remaining == 0) {
                usb_ep0_ctx.state = USB_STATE_WAIT_STATUS_OUT;
                UEP0_T_LEN = 0x00;
                UEP0_CTRL = bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
                return;
            }

            uint8_t previous_length = UEP0_T_LEN;
            uint8_t length = usb_ep0_prepare_descriptor();

            // 要求長より短いDescriptorが64バイト境界で終わった場合はZLPを送る。
            if (length == 0) {
                if (previous_length == USB_EP0_BUFFER_SIZE) {
                    usb_ep0_ctx.state = USB_STATE_SEND_CONFIGURATION_ZLP;
                    UEP0_T_LEN = 0;
                    UEP0_CTRL = ((UEP0_CTRL ^ bUEP_T_TOG) & bUEP_T_TOG) |
                                bUEP_R_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
                } else {
                    usb_ep0_ctx.state = USB_STATE_WAIT_STATUS_OUT;
                    UEP0_T_LEN = 0;
                    UEP0_CTRL = bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
                }
                return;
            }

            uint8_t next_t_toggle = (UEP0_CTRL ^ bUEP_T_TOG) & bUEP_T_TOG;
            UEP0_T_LEN = length;
            UEP0_CTRL = next_t_toggle | bUEP_R_TOG | UEP_R_RES_NAK | UEP_T_RES_ACK;
            return;

        case USB_STATE_SEND_CONFIGURATION_ZLP:
            usb_ep0_ctx.state = USB_STATE_WAIT_STATUS_OUT;
            UEP0_T_LEN = 0;
            UEP0_CTRL = bUEP_R_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            return;

        case USB_STATE_WAIT_SET_CONFIGURATION:
            usb_ep0_ctx.state = USB_STATE_IDLE;
            usb_ep0_ctx.configuration = usb_ep0_ctx.configuration_pending.configuration_candidate;

            UEP0_T_LEN = 0;
            UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
            return;

        default:
            break;
    }
}
