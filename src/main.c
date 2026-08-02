#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/ch552e.h"
#include "isr_util.h"

// EP0のDMA転送先xRAMアドレス
#define USB_ENDPOINT0_ADDR 0x0000

typedef enum {
    EP0_IDLE,
    EP0_DATA_IN_DEVICE_DESCRIPTOR,
    EP0_STATUS_OUT,
    EP0_STATUS_IN_SET_ADDRESS,
    EP0_STALLED,
} ep0_state_t;

static ep0_state_t ep0_state;
static uint8_t pending_address;
static uint8_t current_configuration;
static const uint8_t __code* ep0_tx_ptr;
static uint16_t ep0_tx_remaining;

__xdata __at(USB_ENDPOINT0_ADDR)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_packet_t;

static const uint8_t __code device_descriptor[] = {
    18,   /* bLength */
    0x01, /* DEVICE descriptor */
    0x00,
    0x02, /* USB 2.00 */
    0x00, /* device class */
    0x00, /* subclass */
    0x00, /* protocol */
    64,   /* EP0 maximum packet size */
    0x09,
    0x12, /* VID: 仮値 */
    0x01,
    0x00, /* PID: 仮値 */
    0x00,
    0x01, /* device release */
    0,    /* manufacturer string */
    0,    /* product string */
    0,    /* serial string */
    1,    /* configurations */
};

/** エンドポイント0の状態をリセットする */
void usb_ep0_reset(void) {
    ep0_state = EP0_IDLE;
    pending_address = 0x00;
    current_configuration = 0x00;
    ep0_tx_ptr = NULL;
    ep0_tx_remaining = 0x00;
}

/** エンドポイント0をSTALL状態にする */
static void usb_ep0_stall(void) {
    UEP0_T_LEN = 0;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
    ep0_state = EP0_STALLED;
}

/** EP0送信ポインタ上のデータをxRAM上のDMA領域にコピー */
static void usb_ep0_prepare_in_packet(void) {
    uint8_t length = ep0_tx_remaining > 64 ? 64 : (uint8_t)ep0_tx_remaining;

    for (uint8_t i = 0; i < length; i++) {
        ep0_buffer[i] = ep0_tx_ptr[i];
    }

    ep0_tx_ptr += length;
    ep0_tx_remaining -= length;

    UEP0_T_LEN = length;

    /*
     * SETUP後の最初のData INはDATA1。
     * OUT側はStatus OUT受信用にACKでもよいが、
     * Data IN中はNAKにしておく方が状態が明確。
     */
    UEP0_CTRL =
        UEP_R_RES_NAK |
        UEP_T_RES_ACK |
        bUEP_T_TOG;
}

void usb_ep0_setup(uint8_t length) {
    if (length != 8) {
        usb_ep0_stall();
        return;
    }

    UEP0_T_LEN = 0;

    /*
     * SETUP受信後:
     * 次のData/Status INはDATA1
     */
    UEP0_CTRL =
        UEP_R_RES_NAK |
        UEP_T_RES_NAK |
        bUEP_T_TOG;

    // EP0のバッファにDMA転送されてきたデータをsetupパケットにキャスト
    const usb_setup_packet_t __xdata* setup =
        (const usb_setup_packet_t __xdata*)ep0_buffer;

    // 新しいSETUPは以前のコントロール転送を中断するため、状態をここで初期化する
    ep0_tx_ptr = NULL;
    ep0_tx_remaining = 0;

    if (
        setup->bmRequestType == 0b00000000 &&
        setup->bRequest == USB_REQ_SET_ADDRESS &&
        setup->wIndex == 0 &&
        setup->wLength == 0 &&
        setup->wValue <= 127) {
        // デバイスアドレス候補を保持
        pending_address = (uint8_t)(setup->wValue & MASK_USB_ADDR);

        // ゼロ長のINを返す
        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_NAK | UEP_T_RES_ACK | bUEP_T_TOG;

        // 状態遷移
        ep0_state = EP0_STATUS_IN_SET_ADDRESS;

        return;
    }

    if (
        setup->bmRequestType == 0x80 &&
        setup->bRequest == USB_REQ_GET_DESCRIPTOR &&
        (uint8_t)(setup->wValue >> 8) == 0x01 &&
        (uint8_t)setup->wValue == 0 &&
        setup->wIndex == 0) {
        uint16_t send_length = sizeof(device_descriptor);

        if (setup->wLength < send_length) {
            send_length = setup->wLength;
        }

        ep0_tx_ptr = device_descriptor;
        ep0_tx_remaining = send_length;
        ep0_state = EP0_DATA_IN_DEVICE_DESCRIPTOR;

        usb_ep0_prepare_in_packet();
        return;
    }

    usb_ep0_stall();
}

void usb_ep0_in(void) {
    if (ep0_state == EP0_STATUS_IN_SET_ADDRESS) {
        // アドレス確定
        USB_DEV_AD =
            (USB_DEV_AD & bUDA_GP_BIT) |
            (pending_address & MASK_USB_ADDR);

        pending_address = 0x00;

        UEP0_T_LEN = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

        // idleへ遷移
        ep0_state = EP0_IDLE;
        return;
    }

    if (ep0_tx_remaining != 0) {
        /*
         * 次パケットを準備する場合は、
         * T_TOGを反転してからACK。
         */
        UEP0_CTRL ^= bUEP_T_TOG;
        usb_ep0_prepare_in_packet();
        return;
    }

    /*
     * Data IN完了。
     * 次はホストからStatus OUT ZLPを待つ。
     *
     * Status OUTはDATA1。
     */
    UEP0_T_LEN = 0;
    UEP0_CTRL =
        bUEP_R_TOG |
        UEP_R_RES_ACK |
        UEP_T_RES_NAK;

    ep0_state = EP0_STATUS_OUT;
    return;
}

void usb_ep0_out(uint8_t length) {
    if (ep0_state == EP0_STATUS_OUT && length == 0) {
        UEP0_T_LEN = 0;

        /*
         * 次のSETUPを受信可能に戻す。
         * RX/TX toggleはDATA0へ戻す。
         */
        UEP0_CTRL =
            UEP_R_RES_ACK |
            UEP_T_RES_NAK;

        ep0_state = EP0_IDLE;
        return;
    }

    usb_ep0_stall();
}

ISR(INT_NO_USB) {
    if (UIF_TRANSFER) {
        uint8_t status = USB_INT_ST;
        uint8_t endpoint = status & 0b111;
        uint8_t token = (status >> 4) & 0b11;

        if (endpoint == 0) {
            if (token == UIS_TOKEN_SETUP) {
                usb_ep0_setup(USB_RX_LEN);
            } else if (token == UIS_TOKEN_OUT) {
                usb_ep0_out(USB_RX_LEN);
            } else if (token == UIS_TOKEN_IN) {
                usb_ep0_in();
            }
        }

        UIF_TRANSFER = 0;
    }

    if (UIF_BUS_RST) {
        USB_DEV_AD = 0x00;
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP0_T_LEN = 0x00;

        usb_ep0_reset();

        UIF_BUS_RST = 0;
    }

    if (UIF_SUSPEND) {
        // suspend process
        UIF_SUSPEND = 0;
    }
}

int main(void) {
    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    // USBエンドポイント0の初期化

    // - receive toggle: DATA0
    // - transmit toggle: DATA0
    // - SETUP/OUT: ACK
    // - IN: NAK
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

    // エンドポイント0送信バッファ データ長を0に設定
    UEP0_T_LEN = 0x00;

    // デバイスアドレスを0にしておく
    USB_DEV_AD = 0x00;

    // DMA転送アドレスを設定
    UEP0_DMA = USB_ENDPOINT0_ADDR;

    // EP0ステートマシンを初期化
    usb_ep0_reset();

    // USB割込み有効化
    USB_INT_EN = 0b00000111;
    IE_USB = 1;

    // Full-Speed USBポートを有効化
    UDEV_CTRL = bUD_PD_DIS | bUD_PORT_EN;

    // 内部D+プルアップを有効化, DMA有効化
    USB_CTRL =
        bUC_DEV_PU_EN |
        bUC_INT_BUSY |
        bUC_DMA_EN;

    // グローバル割込み有効化
    EA = 1;

    while (1) {
        // loop
    }
}
