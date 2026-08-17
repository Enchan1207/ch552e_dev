#include <ch552e/memory.h>

#include "endpoint_0_private.h"

void usb_ep0_prepare_next_in_packet(usb_ctx_t* ctx) {
    uint16_t remaining = ctx->tx_length - ctx->tx_offset;

    if (remaining == 0) {
        // 送信完了, Status OUTへ遷移 (DATA1を受ける状態)
        ctx->ep0_state = USB_EP0_STATE_STATUS_OUT;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        return;
    }

    uint8_t packet_length =
        remaining > USB_EP0_MAX_PACKET_SIZE
            ? USB_EP0_MAX_PACKET_SIZE
            : remaining;

    memcpy_code_to_xdata(ep0_buffer, ctx->tx_data + ctx->tx_offset, packet_length);

    ctx->tx_offset += packet_length;

    UEP0_T_LEN = packet_length;

    UEP0_CTRL = bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
}
