#ifndef HARDWARE_USB_EP0_CONTEXT_H
#define HARDWARE_USB_EP0_CONTEXT_H

#include <ch552e/io.h>
#include <stdbool.h>
#include <stdint.h>

// MARK: - structs

/** EP0の状態 */
typedef enum {
    /** 初期状態 */
    USB_STATE_INIT,

    /** アイドル (リクエストを処理できる状態) */
    USB_STATE_IDLE,

    /** デバイスアドレス確定待ち */
    USB_STATE_ADDRESS_PENDING,

    /** デバイスディスクリプタ送信中 */
    USB_STATE_GET_DEVICE_DESCRIPTOR,

    /** コンフィギュレーションディスクリプタ送信中 */
    USB_STATE_GET_CONFIGURATION_DESCRIPTOR,
} usb_ep0_state;

/** USBコンテキスト */
typedef struct {
    /** EP0の状態 */
    usb_ep0_state state;

    union {
        struct {
            uint8_t address_candidate;
        } address_pending;
    };

} usb_ep0_ctx_t;

/** SETUPパケット */
typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_packet_t;

// MARK: - globals

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

extern __xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

// MARK: - functions

/** EP0をSTALL状態にする */
inline void usb_ep0_stall(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
    P1_4 = 1;
}

/**
 * @brief SETUPパケットを処理する
 *
 * @param ctx
 * @param packet
 * @return bool 処理成否
 */
bool usb_ep0_handle_setup(usb_ep0_ctx_t* ctx, const usb_setup_packet_t __xdata* packet);

#endif /* HARDWARE_USB_EP0_CONTEXT_H */
