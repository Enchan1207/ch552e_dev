#ifndef HARDWARE_USB_EP0_EP0_H
#define HARDWARE_USB_EP0_EP0_H

#include <stdint.h>

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

#endif /* HARDWARE_USB_EP0_EP0_H */
