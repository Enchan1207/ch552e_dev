#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal.h"

/**
 * @brief 残り送信データ数を進める
 *
 * @param ctx
 * @param size
 * @return uint16_t
 */
static inline uint16_t consume_buffer(usb_ep0_ctx_t* ctx, uint16_t size) {
    if (ctx->configuration_send_stream.remaining <= size) {
        ctx->configuration_send_stream.remaining = 0;
    } else {
        ctx->configuration_send_stream.remaining -= size;
    }

    return ctx->configuration_send_stream.remaining;
}

/**
 * @brief 次のディスクリプタを準備する
 *
 * @param ctx
 * @return bool ディスクリプタ列挙ループを続行可能かどうか
 */
static inline bool prepare_next_descriptor(usb_ep0_ctx_t* ctx) {
    // オフセットがディスクリプタサイズに達していない = まだ送りきっていない, 現状維持
    if (ctx->configuration_send_stream.offset <
        ctx->configuration_send_stream.descriptor_size) {
        return true;
    }

    // 次の従属ディスクリプタを取得できる?
    uint8_t size = 0;
    const __code uint8_t* child_descriptor = usb_get_interface_child_descriptor(
        ctx->configuration_send_stream.if_index,
        ctx->configuration_send_stream.index,
        &size);

    if (child_descriptor != NULL) {
        ctx->configuration_send_stream.index++;
        ctx->configuration_send_stream.descriptor = child_descriptor;
        ctx->configuration_send_stream.descriptor_size = size;
        ctx->configuration_send_stream.offset = 0;
        return true;
    }

    // 次のインタフェースディスクリプタを取得できる?
    ctx->configuration_send_stream.if_index++;
    ctx->configuration_send_stream.index = 0;
    usb_interface_descriptor_ptr interface_descriptor = usb_get_interface_descriptor(ctx->configuration_send_stream.if_index);

    if (interface_descriptor != NULL) {
        ctx->configuration_send_stream.descriptor = interface_descriptor;
        ctx->configuration_send_stream.descriptor_size = interface_descriptor->bLength;
        ctx->configuration_send_stream.offset = 0;
        return true;
    }

    // 次のディスクリプタはない
    return false;
}

uint8_t usb_ep0_prepare_descriptor(usb_ep0_ctx_t* ctx) {
    if (ctx->state != USB_STATE_SEND_CONFIGURATION_DESCRIPTOR) {
        return 0x00;
    }

    uint8_t filled_bytes = 0x00;

    // 残送信データがなくなるか、バッファが埋まるまで繰り返す
    while (ctx->configuration_send_stream.remaining > 0 && filled_bytes < USB_EP0_BUFFER_SIZE) {
        // 現在のディスクリプタにオフセットを足してコピー元位置、バッファに書込み済バイト数を足してコピー先位置を特定
        const __code uint8_t* src = ctx->configuration_send_stream.descriptor + ctx->configuration_send_stream.offset;
        __xdata uint8_t* dest = ep0_buffer + filled_bytes;

        // バッファを飛び出さぬようにコピー長を決定
        uint8_t current_descriptor_size = ctx->configuration_send_stream.descriptor_size - ctx->configuration_send_stream.offset;
        uint8_t copy_size_max = USB_EP0_BUFFER_SIZE - filled_bytes;
        uint8_t copy_size = current_descriptor_size > copy_size_max ? copy_size_max : current_descriptor_size;

        // バッファにコピー
        if (copy_size > 0) {
            memcpy_code_to_xdata(dest, src, copy_size);
            filled_bytes += copy_size;
            consume_buffer(ctx, copy_size);
            ctx->configuration_send_stream.offset = copy_size;
        }

        // ディスクリプタとオフセットを更新
        bool can_continue = prepare_next_descriptor(ctx);
        if (!can_continue) {
            break;
        }
    }

    return filled_bytes;
}
