#ifndef HARDWARE_USB_EP0_CONTEXT_H
#define HARDWARE_USB_EP0_CONTEXT_H

#include <ch552e/io.h>
#include <stdbool.h>
#include <stdint.h>

// MARK: - macros

#define min(lhs, rhs) (lhs > rhs ? rhs : lhs)
#define max(lhs, rhs) (lhs > rhs ? lhs : rhs)

// MARK: - structs

/** EP0の状態 */
typedef enum {
    /** 初期状態 */
    USB_STATE_INIT,

    /** デバイスアドレス確定待ち */
    USB_STATE_WAIT_DEVICE_ADDRESS,

    /** コンフィギュレーション設定確認待ち */
    USB_STATE_WAIT_SET_CONFIGURATION,

    /** デバイスディスクリプタ送信中 */
    USB_STATE_SEND_DEVICE_DESCRIPTOR,

    /** コンフィギュレーションディスクリプタ送信中 */
    USB_STATE_SEND_CONFIGURATION_DESCRIPTOR,

    /** StatusステージでOUTを待機中 */
    USB_STATE_WAIT_STATUS_OUT,

    /** アイドル (リクエストを処理できる状態) */
    USB_STATE_IDLE,
} usb_ep0_state;

/** configストリームのフェーズ */
typedef enum {
    /** コンフィギュレーションディスクリプタを送信 */
    USB_CONFIGURATION_STREAM_PHASE_CONFIGURATION,

    /** インタフェースディスクリプタを送信 */
    USB_CONFIGURATION_STREAM_PHASE_INTERFACE,

    /** インタフェース従属ディスクリプタを送信 */
    USB_CONFIGURATION_STREAM_PHASE_INTERFACE_CHILD
} configuration_stream_phase_t;

/** USBコンテキスト */
typedef struct {
    /** EP0の状態 */
    usb_ep0_state state;

    union {
        struct {
            /** デバイスアドレス候補 */
            uint8_t address_candidate;
        } address_pending;

        struct {
            /** コンフィギュレーション候補 */
            uint8_t configuration_candidate;
        } configuration_pending;

        struct {
            /** 現在のストリームフェーズ */
            configuration_stream_phase_t phase;

            /** 現在送信中のディスクリプタのポインタ */
            const uint8_t __code* descriptor;

            /** 現在送信中のディスクリプタの長さ */
            uint8_t descriptor_size;

            /** 要求されているデータの全体長 */
            uint16_t remaining;

            /** 現在送信中のディスクリプタをどこまで送信したか */
            uint8_t offset;

            /** インタフェースディスクリプタのインデックス */
            uint8_t if_index;

            /** インタフェース従属ディスクリプタのインデックス */
            uint8_t index;
        } configuration_stream;
    };

    /** 現在選択されているコンフィギュレーション */
    uint8_t configuration;

} usb_ep0_ctx_t;

/** USB EP0コンテキストのポインタ */
typedef __idata usb_ep0_ctx_t* usb_ep0_ctx_ptr;

/** SETUPパケット */
typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_packet_t;

/** SETUPパケットのポインタ */
typedef const usb_setup_packet_t __idata* usb_setup_packet_ptr;

// MARK: - globals

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_BUFFER_ADDRESS 0x0000

/** EP0のDMAバッファ長 */
#define USB_EP0_BUFFER_SIZE 64

extern __xdata __at(USB_EP0_BUFFER_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[USB_EP0_BUFFER_SIZE];

/** EP0コンテキスト */
extern __idata usb_ep0_ctx_t* ctx;

// MARK: - functions

/** EP0をSTALL状態にする */
inline void usb_ep0_stall(void) {
    UEP0_T_LEN = 0x00;
    UEP0_CTRL = UEP_R_RES_STALL | UEP_T_RES_STALL;
    P1_4 = 1;
}

/**
 * @brief SETUPを処理する
 *
 * @param packet
 * @return bool 処理成否
 */
bool usb_ep0_handle_setup(usb_setup_packet_ptr packet);

/**
 * @brief INを処理する
 *
 */
void usb_ep0_handle_in(void);

/**
 * @brief OUTを処理する
 *
 * @param length
 */
void usb_ep0_handle_out(uint8_t length);

/**
 * @brief ディスクリプタ返答用のデータを準備する
 * @note コンフィギュレーションディスクリプタ送信要求に対する応答に利用することを想定しています。
 *
 * @return 実際に詰めたデータの長さ
 */
uint8_t usb_ep0_prepare_descriptor(void);

#endif /* HARDWARE_USB_EP0_CONTEXT_H */
