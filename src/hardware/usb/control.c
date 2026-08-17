#include <ch552e/io.h>

#include "hardware/usb/usb.h"
#include "usb_private.h"

usb_ctx_t ctx = {
    .ep0_state = USB_EP0_STATE_IDLE,
    .device_address_candidate = -1,
    .latest_request_type = -1,
};

void usb_init(void) {
    irq_disable();
    IE_USB = 0;
    usb_reset_context();

    USB_CTRL = 0x00;

    // USBエンドポイント0の初期化
    UEP0_DMA = USB_EP0_DMA_ADDRESS;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP0_T_LEN = 0x00;
    USB_DEV_AD = 0x00;

    // 内部プルダウン無効
    UDEV_CTRL = bUD_PD_DIS;

    USB_CTRL =
        bUC_DEV_PU_EN |  // 内部D+プルアップ機能を利用する
        bUC_INT_BUSY |   // UIF_TRANSFERがnegateされるまでR_NAK
        bUC_DMA_EN;      // USB DMAを利用する

    // ポート有効化
    UDEV_CTRL |= bUD_PORT_EN;

    // USB割込み初期化
    USB_INT_FG = 0xFF;
    USB_INT_EN =
        // bUIE_SUSPEND |
        // bUIE_DEV_NAK |
        bUIE_TRANSFER |  // 転送完了イベント
        bUIE_BUS_RST;    // バスリセットイベント

    IE_USB = 1;
}

void usb_reset_context(void) {
    ctx.ep0_state = USB_EP0_STATE_IDLE;
    ctx.device_address_candidate = -1,
    ctx.latest_request_type = REQ_NONE;
}
