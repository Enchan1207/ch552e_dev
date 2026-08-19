#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal.h"

static bool retrieve_interface_descriptor(usb_ep0_ctx_ptr ctx) {
    usb_interface_descriptor_ptr interface_descriptor = usb_get_interface_descriptor(ctx->configuration_stream.if_index);
    if (interface_descriptor == NULL) {
        return false;
    }

    ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_INTERFACE;
    ctx->configuration_stream.descriptor = interface_descriptor;
    ctx->configuration_stream.descriptor_size = interface_descriptor->bLength;
    ctx->configuration_stream.offset = 0;
    ctx->configuration_stream.index = 0;
    return true;
}

static bool move_to_next_interface_child_descriptor(usb_ep0_ctx_ptr ctx) {
    uint8_t size = 0;
    const __code uint8_t* child_descriptor = usb_get_interface_child_descriptor(
        ctx->configuration_stream.if_index,
        ctx->configuration_stream.index,
        &size);

    // 取得できなければ次のI/Fディスクリプタに進む
    if (child_descriptor == NULL) {
        return false;
    }

    ctx->configuration_stream.phase = USB_CONFIGURATION_STREAM_PHASE_INTERFACE_CHILD;
    ctx->configuration_stream.index++;
    ctx->configuration_stream.descriptor = child_descriptor;
    ctx->configuration_stream.descriptor_size = size;
    ctx->configuration_stream.offset = 0;
    return true;
}

/**
 * @brief 次のディスクリプタに移動する
 *
 * @param ctx
 * @return bool ディスクリプタ列挙ループを続行可能かどうか
 */
static inline bool move_to_next_descriptor(usb_ep0_ctx_ptr ctx) {
    // オフセットがディスクリプタサイズに達していない = まだ送りきっていない, 現状維持
    if (ctx->configuration_stream.offset <
        ctx->configuration_stream.descriptor_size) {
        return true;
    }

    switch (ctx->configuration_stream.phase) {
        case USB_CONFIGURATION_STREAM_PHASE_CONFIGURATION: {
            retrieve_interface_descriptor(ctx);
            return true;
        }

        case USB_CONFIGURATION_STREAM_PHASE_INTERFACE: {
            bool moved = move_to_next_interface_child_descriptor(ctx);
            if (!moved) {
                ctx->configuration_stream.if_index++;
                return retrieve_interface_descriptor(ctx);
            }

            return true;
        }

        case USB_CONFIGURATION_STREAM_PHASE_INTERFACE_CHILD: {
            bool moved = move_to_next_interface_child_descriptor(ctx);
            if (!moved) {
                ctx->configuration_stream.if_index++;
                return retrieve_interface_descriptor(ctx);
            }

            return true;
        }

        default:
            return false;
    }
}

static inline uint8_t min(uint8_t lhs, uint8_t rhs) {
    return lhs > rhs ? rhs : lhs;
}

uint8_t usb_ep0_prepare_descriptor(usb_ep0_ctx_ptr ctx) {
    if (ctx->state != USB_STATE_SEND_CONFIGURATION_DESCRIPTOR) {
        return 0;
    }

    if (ctx->configuration_stream.descriptor == NULL) {
        return 0;
    }

    if (ctx->configuration_stream.remaining == 0) {
        return 0;
    }

    // バッファにコピーしたバイト数
    uint8_t filled_bytes = 0x00;

    // 全て送信し終えるか、バッファがいっぱいになるまで続ける
    while (ctx->configuration_stream.remaining > 0 && filled_bytes < USB_EP0_BUFFER_SIZE) {
        uint8_t offset = ctx->configuration_stream.offset;
        uint8_t descriptor_size = ctx->configuration_stream.descriptor_size;

        // コピー元とコピー先を特定
        const __code uint8_t* src = ctx->configuration_stream.descriptor + offset;
        __xdata uint8_t* dest = ep0_buffer + filled_bytes;

        // コピー長を決定
        uint8_t desciptor_remaining = descriptor_size - offset;
        uint8_t buffer_remaining = USB_EP0_BUFFER_SIZE - filled_bytes;
        uint8_t copy_size = min(desciptor_remaining, buffer_remaining);
        if (ctx->configuration_stream.remaining < copy_size) {
            copy_size = ctx->configuration_stream.remaining;
        }

        memcpy_code_to_xdata(dest, src, copy_size);

        ctx->configuration_stream.offset += copy_size;
        ctx->configuration_stream.remaining -= copy_size;
        filled_bytes += copy_size;

        // 全部コピーした?
        if (desciptor_remaining != copy_size) {
            continue;
        }

        // 次のディスクリプタへ
        bool has_next_descriptor = move_to_next_descriptor(ctx);
        if (!has_next_descriptor) {
            break;
        }
    }

    return filled_bytes;
}
