#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <stddef.h>

#include "hardware/usb/usb.h"
#include "usb_private.h"

usb_ctx_t ctx_internal;

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
    ctx_internal.ep0_state = USB_EP0_STATE_IDLE;
    ctx_internal.device_address_candidate = 0x00;
    ctx_internal.tx_data = NULL;
    ctx_internal.tx_length = 0;
    ctx_internal.tx_offset = 0;
}

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
        usb_ep0_handle_packet(&ctx_internal, token, length);
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
