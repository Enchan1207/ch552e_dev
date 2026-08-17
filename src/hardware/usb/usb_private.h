#ifndef HARDWARE_USB_PRIVATE_H
#define HARDWARE_USB_PRIVATE_H

#include <ch552e/io.h>
#include <stdint.h>

#include "usb_setup_fifo.h"

// MARK: state

/** USBコンテキスト */
typedef struct usb_ctx_t {
    /**
     * @brief デバイスアドレス候補
     * @note 0~127は有効アドレス、-1は候補がないことを示します。
     */
    int8_t device_address_candidate;

    /**
     * @brief 最後に受け取ったSETUPパケットの `bRequest` 値
     * @note -1はSETUPパケットを一度も受け取っていない事を示します。
     */
    int8_t latest_request_type;
} usb_ctx_t;

extern usb_ctx_t ctx;

/**
 * @brief USBコンテキストをリセットする
 */
void usb_reset_context(void);

// MARK: Endpoint #0

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

extern __xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

/**
 * @brief EP0のパケットを処理する
 *
 * @param token パケットのトークン
 * @param length パケットの長さ
 */
void usb_ep0_handle_packet(uint8_t token, uint8_t length);

// MARK: - bmRequestType

#define MASK_MREQ_DIRECTION 0b10000000
#define MREQ_DIRECTION_H2D 0b00000000
#define MREQ_DIRECTION_D2H 0b10000000

#define MASK_MREQ_TYPE 0b01100000
#define MREQ_TYPE_STANDARD 0b00000000
#define MREQ_TYPE_CLASS 0b00100000
#define MREQ_TYPE_VENDOR 0b01000000

#define MASK_MREQ_TARGET 0b00011111
#define MREQ_TARGET_DEVICE 0b00000000
#define MREQ_TARGET_INTERFACE 0b00000001
#define MREQ_TARGET_ENDPOINT 0b00000010
#define MREQ_TARGET_OTHER 0b00000011

// MARK: - bRequest

#define REQ_SET_ADDRESS 0x05
#define REQ_GET_DESCRIPTOR 0x06
#define REQ_NONE -1

#endif /* HARDWARE_USB_PRIVATE_H */
