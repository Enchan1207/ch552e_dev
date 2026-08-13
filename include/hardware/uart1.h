#ifndef HARDWARE_UART1_H
#define HARDWARE_UART1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef UART1_RX_BUFSIZE
/// @brief 受信バッファサイズ
#define UART1_RX_BUFSIZE 32
#endif

#if (UART1_RX_BUFSIZE & (UART1_RX_BUFSIZE - 1)) != 0
#error "UART1_RX_BUFSIZE must be a power of 2"
#endif

#ifndef UART1_BAUD_RATE
/// @brief UART1ボーレート
#define UART1_BAUD_RATE 9600
#endif

/// @brief UART1コンテキスト
typedef struct uart1_ctx_t uart1_ctx_t;

/// @brief UART1
extern uart1_ctx_t* uart1;

/// @brief UART1通信開始
/// @note ボーレートはマクロ UART_BAUD_RATE により設定します。
void uart_begin(void);

/// @brief UART送信 (バイナリデータ)
/// @param ctx
/// @param data
/// @param len
/// @note 与えられたデータの送信が完了するまでブロックしま
void uart_write(uart1_ctx_t* ctx, const uint8_t* const data, size_t len);

/// @brief USART送信 (バイナリデータ, ノンブロッキング)
/// @param ctx
/// @param data
/// @param len
/// @note 現在実行中の送信がある場合は、完了するまでブロックします。
void uart_write_noblock(uart1_ctx_t* ctx, const uint8_t* const data, size_t len);

/// @brief USART送信 (文字列)
/// @param ctx
/// @param str
/// @note 与えられたデータの送信が完了するまでブロックします。
void uart_print(uart1_ctx_t* ctx, const char* const str);

/// @brief USART送信 (文字列, ノンブロッキング)
/// @param ctx
/// @param str
/// @note 現在実行中の送信がある場合は、完了するまでブロックします。
void uart_print_noblock(uart1_ctx_t* ctx, const char* const str);

/// @brief USART受信
/// @return 受信結果
/// @note データを受信するまでブロックします。
uint8_t uart_read(uart1_ctx_t* ctx);

/// @brief 受信バッファ状態確認
/// @return データを受信していれば true が返ります。
bool uart_available(uart1_ctx_t* ctx);

#endif /* HARDWARE_UART1_H */
