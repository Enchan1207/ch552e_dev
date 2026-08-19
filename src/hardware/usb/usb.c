#include "hardware/usb/usb.h"

#include <ch552e/io.h>
#include <ch552e/memory.h>
#include <stddef.h>

#include "ep0/ep0.h"

void usb_init(void) {
    irq_disable();
    IE_USB = 0;

    USB_CTRL = 0x00;

    // USBエンドポイント0の初期化
    usb_ep0_init();

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
