#include <ch552e/memory.h>
#include <hardware/usb/usb.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal.h"

/** 次に送るディスクリプタをストリームへ設定する */
static void select_descriptor(usb_descriptor_ptr descriptor) {
    usb_ep0_ctx.config_stream.cursor = descriptor;
    usb_ep0_ctx.config_stream.remaining = descriptor[0];
}

/** Configuration 直下の Entry を取得する */
static bool move_to_entry(void) {
    usb_descriptor_ptr entry = usb_get_configuration_entry(usb_ep0_ctx.config_stream.entry_index);
    if (entry == NULL) {
        return false;
    }

    select_descriptor(entry);
    usb_ep0_ctx.config_stream.child_index = 0;
    usb_ep0_ctx.config_stream.phase = entry[1] == USB_DESCRIPTOR_TYPE_INTERFACE
                                          ? USB_CONFIG_PHASE_CHILD
                                          : USB_CONFIG_PHASE_ENTRY;
    return true;
}

/** 現在の Interface Entry に属する次の Child を取得する */
static bool move_to_child(void) {
    if (usb_ep0_ctx.config_stream.child_index == UINT8_MAX) {
        return false;
    }

    usb_descriptor_ptr child = usb_get_configuration_entry_child(
        usb_ep0_ctx.config_stream.entry_index,
        usb_ep0_ctx.config_stream.child_index);
    if (child == NULL) {
        return false;
    }

    select_descriptor(child);
    usb_ep0_ctx.config_stream.child_index++;
    return true;
}

/** Configuration → Entry → Interface Child の順に進む */
static bool move_to_next_descriptor(void) {
    if (usb_ep0_ctx.config_stream.phase == USB_CONFIG_PHASE_HEADER) {
        return move_to_entry();
    }

    if (usb_ep0_ctx.config_stream.phase == USB_CONFIG_PHASE_CHILD && move_to_child()) {
        return true;
    }

    if (usb_ep0_ctx.config_stream.entry_index == UINT8_MAX) {
        return false;
    }

    usb_ep0_ctx.config_stream.entry_index++;
    return move_to_entry();
}

uint8_t usb_ep0_prepare_descriptor(void) {
    if (usb_ep0_ctx.state != USB_STATE_SEND_CONFIGURATION_DESCRIPTOR) {
        return 0;
    }

    if (usb_ep0_ctx.config_stream.cursor == NULL) {
        return 0;
    }

    // バッファにコピーしたバイト数
    uint8_t filled_bytes = 0x00;

    // 全て送信し終えるか、バッファがいっぱいになるまで続ける
    while (usb_ep0_ctx.config_stream.cursor != NULL &&
           usb_ep0_ctx.config_stream.request_remaining != 0 &&
           usb_ep0_ctx.config_stream.total_remaining != 0 &&
           filled_bytes < USB_EP0_BUFFER_SIZE) {
        // コピー長を決定
        uint8_t buffer_remaining = USB_EP0_BUFFER_SIZE - filled_bytes;
        uint8_t copy_size = min(usb_ep0_ctx.config_stream.remaining, buffer_remaining);
        copy_size = min(copy_size, usb_ep0_ctx.config_stream.request_remaining);
        copy_size = min(copy_size, usb_ep0_ctx.config_stream.total_remaining);

        // コピー元とコピー先を特定
        __xdata uint8_t* dest = ep0_buffer + filled_bytes;

        memcpy_code_to_xdata(dest, usb_ep0_ctx.config_stream.cursor, copy_size);

        usb_ep0_ctx.config_stream.cursor += copy_size;
        usb_ep0_ctx.config_stream.remaining -= copy_size;
        usb_ep0_ctx.config_stream.request_remaining -= copy_size;
        usb_ep0_ctx.config_stream.total_remaining -= copy_size;
        filled_bytes += copy_size;

        if (usb_ep0_ctx.config_stream.request_remaining == 0 ||
            usb_ep0_ctx.config_stream.total_remaining == 0) {
            break;
        }

        // 全部コピーした?
        if (usb_ep0_ctx.config_stream.remaining != 0) {
            continue;
        }

        // 次のディスクリプタへ
        bool has_next_descriptor = move_to_next_descriptor();
        if (!has_next_descriptor) {
            usb_ep0_ctx.config_stream.cursor = NULL;
            break;
        }
    }

    return filled_bytes;
}
