#ifndef HARDWARE_USB_ENDPOINT_0_PRIVATE_H
#define HARDWARE_USB_ENDPOINT_0_PRIVATE_H

#include <ch552e/io.h>

#include "../usb_private.h"
#include "hardware/usb/usb.h"

/** EP0パケット最大サイズ */
#define USB_EP0_MAX_PACKET_SIZE 64

/** EP0をSTALL状態にする */
inline void usb_ep0_stall(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
    P1_4 = 1;
}

/**
 * @brief EP0のSETUPパケットを処理する
 *
 * @param packet パケット
 * @return int8_t 送信するデータ長 (負数はエラー)
 */
int8_t usb_ep0_handle_setup(usb_ctx_t* ctx, const usb_setup_packet_t __xdata* packet);

/**
 * @brief EP0で次に送信するデータを準備する
 */
void usb_ep0_prepare_next_in_packet(usb_ctx_t* ctx);

/**
 * @brief EP0のINパケットを処理する
 */
void usb_ep0_handle_in(usb_ctx_t* ctx);

/**
 * @brief EP0のOUTパケットを処理する
 */
void usb_ep0_handle_out(usb_ctx_t* ctx, uint8_t length);

#endif /* HARDWARE_USB_ENDPOINT_0_PRIVATE_H */
