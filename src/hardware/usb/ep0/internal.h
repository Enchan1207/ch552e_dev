#ifndef HARDWARE_USB_EP0_CONTEXT_H
#define HARDWARE_USB_EP0_CONTEXT_H

#include <ch552e/io.h>
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

// MARK: - globals

/** EP0コンテキストポインタ */
extern usb_ep0_ctx_t* usb_ep0_ctx;

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

extern __xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

// MARK: - functions

/**
 * @brief EP0を初期化
 */
void usb_ep0_init(void);

/**
 * @brief EP0バス状態をリセット
 */
void usb_ep0_reset(void);

/**
 * @brief EP0のパケットを処理する
 *
 * @param token パケットのトークン
 * @param length パケットの長さ
 */
void usb_ep0_handle_packet(uint8_t token, uint8_t length);

#endif /* HARDWARE_USB_EP0_CONTEXT_H */
