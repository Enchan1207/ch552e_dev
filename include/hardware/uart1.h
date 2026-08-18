#ifndef HARDWARE_UART1_H
#define HARDWARE_UART1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ch552e/interrupt.h"

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

ISR(INT_NO_UART1);

/// @brief UART1通信開始
/// @note ボーレートはマクロ UART_BAUD_RATE により設定します。
void uart_begin(void);

/// @brief UART送信 (バイナリデータ)
/// @param data
/// @param len
/// @note 与えられたデータの送信が完了するまでブロックします。
void uart_write(const uint8_t* const data, size_t len);

/// @brief USART送信 (バイナリデータ, ノンブロッキング)
/// @param data
/// @param len
/// @note 現在実行中の送信がある場合は、完了するまでブロックします。
void uart_write_noblock(const uint8_t* const data, size_t len);

/// @brief UART送信 (単一バイト)
/// @param data
/// @note 与えられたデータの送信が完了するまでブロックします。
void uart_write_byte(char data);

/// @brief USART送信 (文字列)
/// @param str
/// @note 与えられたデータの送信が完了するまでブロックします。
void uart_print(const char* const str);

/// @brief USART送信 (文字列, ノンブロッキング)
/// @param str
/// @note 現在実行中の送信がある場合は、完了するまでブロックします。
void uart_print_noblock(const char* const str);

/// @brief USART受信
/// @return 受信結果
/// @note データを受信するまでブロックします。
uint8_t uart_read(void);

/// @brief 受信バッファ状態確認
/// @return データを受信していれば true が返ります。
bool uart_available(void);

#endif /* HARDWARE_UART1_H */
