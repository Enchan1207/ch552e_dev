#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal.h"

/**
 * @brief 次のインタフェースディスクリプタをストリームに設定する
 *
 * @return 次のインタフェースディスクリプタが存在した場合は true
 */
static bool move_to_interface_descriptor(void) {
    usb_interface_descriptor_ptr interface_descriptor = usb_get_interface_descriptor(
        usb_ep0_ctx->config_stream_v2.indices.interface);
    if (interface_descriptor == NULL) {
        return false;
    }

    usb_ep0_ctx->config_stream_v2.cursor = interface_descriptor;
    usb_ep0_ctx->config_stream_v2.remaining = interface_descriptor->bLength;
    usb_ep0_ctx->config_stream_v2.indices.child = 0;
    return true;
}

/**
 * @brief 現在のインタフェースの次の従属ディスクリプタをストリームに設定する
 *
 * @return 次の従属ディスクリプタが存在した場合は true
 */
static bool move_to_interface_child_descriptor(void) {
    const __code uint8_t* child_descriptor = usb_get_interface_child_descriptor(
        usb_ep0_ctx->config_stream_v2.indices.interface,
        usb_ep0_ctx->config_stream_v2.indices.child,
        &usb_ep0_ctx->config_stream_v2.remaining);

    if (child_descriptor == NULL) {
        return false;
    }

    usb_ep0_ctx->config_stream_v2.cursor = child_descriptor;
    usb_ep0_ctx->config_stream_v2.indices.child++;
    return true;
}

/**
 * @brief 次のディスクリプタに移動する
 *
 * @return bool ディスクリプタ列挙ループを続行可能かどうか
 */
static inline bool move_to_next_descriptor(void) {
    // child index 31 はコンフィギュレーションディスクリプタ送信中を表す。
    if (usb_ep0_ctx->config_stream_v2.indices.child == 0b00011111) {
        return move_to_interface_descriptor();
    }

    if (move_to_interface_child_descriptor()) {
        return true;
    }

    // 3 bit の interface index で表現できるのは 0-7。
    if (usb_ep0_ctx->config_stream_v2.indices.interface == 0b00000111) {
        return false;
    }

    usb_ep0_ctx->config_stream_v2.indices.interface++;
    return move_to_interface_descriptor();
}

uint8_t usb_ep0_prepare_descriptor(void) {
    if (usb_ep0_ctx->state != USB_STATE_SEND_CONFIGURATION_DESCRIPTOR) {
        return 0;
    }

    if (usb_ep0_ctx->config_stream_v2.cursor == NULL) {
        return 0;
    }

    // バッファにコピーしたバイト数
    uint8_t filled_bytes = 0x00;

    // 全て送信し終えるか、バッファがいっぱいになるまで続ける
    while (usb_ep0_ctx->config_stream_v2.cursor != NULL && filled_bytes < USB_EP0_BUFFER_SIZE) {
        // コピー長を決定
        uint8_t buffer_remaining = USB_EP0_BUFFER_SIZE - filled_bytes;
        uint8_t copy_size = min(usb_ep0_ctx->config_stream_v2.remaining, buffer_remaining);

        // コピー元とコピー先を特定
        __xdata uint8_t* dest = ep0_buffer + filled_bytes;

        memcpy_code_to_xdata(dest, usb_ep0_ctx->config_stream_v2.cursor, copy_size);

        usb_ep0_ctx->config_stream_v2.cursor += copy_size;
        usb_ep0_ctx->config_stream_v2.remaining -= copy_size;
        filled_bytes += copy_size;

        // 全部コピーした?
        if (usb_ep0_ctx->config_stream_v2.remaining != 0) {
            continue;
        }

        // 次のディスクリプタへ
        bool has_next_descriptor = move_to_next_descriptor();
        if (!has_next_descriptor) {
            usb_ep0_ctx->config_stream_v2.cursor = NULL;
            break;
        }
    }

    return filled_bytes;
}
