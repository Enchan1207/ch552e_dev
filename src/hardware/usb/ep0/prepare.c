#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal.h"

static bool retrieve_interface_descriptor(void) {
    usb_interface_descriptor_ptr interface_descriptor = usb_get_interface_descriptor(usb_ep0_ctx->configuration_stream.if_index);
    if (interface_descriptor == NULL) {
        return false;
    }

    usb_ep0_ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_INTERFACE;
    usb_ep0_ctx->configuration_stream.descriptor = interface_descriptor;
    usb_ep0_ctx->configuration_stream.descriptor_size = interface_descriptor->bLength;
    usb_ep0_ctx->configuration_stream.offset = 0;
    usb_ep0_ctx->configuration_stream.index = 0;
    return true;
}

static bool move_to_next_interface_child_descriptor(void) {
    const __code uint8_t* child_descriptor = usb_get_interface_child_descriptor(
        usb_ep0_ctx->configuration_stream.if_index,
        usb_ep0_ctx->configuration_stream.index,
        &usb_ep0_ctx->configuration_stream.descriptor_size);

    // 取得できなければ次のI/Fディスクリプタに進む
    if (child_descriptor == NULL) {
        return false;
    }

    usb_ep0_ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_INTERFACE_CHILD;
    usb_ep0_ctx->configuration_stream.index++;
    usb_ep0_ctx->configuration_stream.descriptor = child_descriptor;
    usb_ep0_ctx->configuration_stream.offset = 0;
    return true;
}

/**
 * @brief 次のディスクリプタに移動する
 *
 * @param ctx
 * @return bool ディスクリプタ列挙ループを続行可能かどうか
 */
static inline bool move_to_next_descriptor(void) {
    // オフセットがディスクリプタサイズに達していない = まだ送りきっていない, 現状維持
    if (usb_ep0_ctx->configuration_stream.offset <
        usb_ep0_ctx->configuration_stream.descriptor_size) {
        return true;
    }

    switch (usb_ep0_ctx->configuration_stream.phase) {
        case USB_CONFIGURATION_STREAM_PHASE_CONFIGURATION: {
            retrieve_interface_descriptor();
            return true;
        }

        // FIXME: ここのfall-throughは気持ち悪い
        case USB_CONFIGURATION_STREAM_PHASE_INTERFACE:
        case USB_CONFIGURATION_STREAM_PHASE_INTERFACE_CHILD: {
            bool moved = move_to_next_interface_child_descriptor();
            if (!moved) {
                usb_ep0_ctx->configuration_stream.if_index++;
                return retrieve_interface_descriptor();
            }

            return true;
        }

        default:
            return false;
    }
}

uint8_t usb_ep0_prepare_descriptor(void) {
    if (usb_ep0_ctx->state != USB_STATE_SEND_CONFIGURATION_DESCRIPTOR) {
        return 0;
    }

    if (usb_ep0_ctx->configuration_stream.descriptor == NULL) {
        return 0;
    }

    if (usb_ep0_ctx->configuration_stream.remaining == 0) {
        return 0;
    }

    // バッファにコピーしたバイト数
    uint8_t filled_bytes = 0x00;

    // 全て送信し終えるか、バッファがいっぱいになるまで続ける
    while (usb_ep0_ctx->configuration_stream.remaining > 0 && filled_bytes < USB_EP0_BUFFER_SIZE) {
        uint8_t offset = usb_ep0_ctx->configuration_stream.offset;
        uint8_t descriptor_size = usb_ep0_ctx->configuration_stream.descriptor_size;

        // コピー長を決定
        uint8_t desciptor_remaining = descriptor_size - offset;
        uint8_t buffer_remaining = USB_EP0_BUFFER_SIZE - filled_bytes;
        uint8_t copy_size = min(desciptor_remaining, buffer_remaining);
        if (usb_ep0_ctx->configuration_stream.remaining < copy_size) {
            copy_size = usb_ep0_ctx->configuration_stream.remaining;
        }

        // コピー元とコピー先を特定
        const __code uint8_t* src = usb_ep0_ctx->configuration_stream.descriptor + offset;
        __xdata uint8_t* dest = ep0_buffer + filled_bytes;

        memcpy_code_to_xdata(dest, src, copy_size);

        usb_ep0_ctx->configuration_stream.offset += copy_size;
        usb_ep0_ctx->configuration_stream.remaining -= copy_size;
        filled_bytes += copy_size;

        // 全部コピーした?
        if (desciptor_remaining != copy_size) {
            continue;
        }

        // 次のディスクリプタへ
        bool has_next_descriptor = move_to_next_descriptor();
        if (!has_next_descriptor) {
            break;
        }
    }

    return filled_bytes;
}
