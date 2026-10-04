#ifndef HARDWARE_UART1_H
#define HARDWARE_UART1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ch552e/interrupt.h"

#ifndef UART1_RX_BUFSIZE
/** UART1受信バッファサイズ */
#define UART1_RX_BUFSIZE 32
#endif

#if (UART1_RX_BUFSIZE & (UART1_RX_BUFSIZE - 1)) != 0
#error "UART1_RX_BUFSIZE must be a power of 2"
#endif

#if (UART1_RX_BUFSIZE < 2) || (UART1_RX_BUFSIZE > 255)
#error "UART1_RX_BUFSIZE must be at least 2 and less than 255"
#endif

#ifndef UART1_TX_BUFSIZE
/** UART1送信バッファサイズ */
#define UART1_TX_BUFSIZE 32
#endif

#if (UART1_TX_BUFSIZE & (UART1_TX_BUFSIZE - 1)) != 0
#error "UART1_TX_BUFSIZE must be a power of 2"
#endif

#if (UART1_TX_BUFSIZE < 2) || (UART1_TX_BUFSIZE > 255)
#error "UART1_TX_BUFSIZE must be at least 2 and less than 255"
#endif

#ifndef UART1_BAUD_RATE
/** UART1ボーレート */
#define UART1_BAUD_RATE 9600
#endif

ISR(INT_NO_UART1);

/**
 * @brief UART1を初期化し、通信を開始する
 *
 * @note ボーレートを変更する場合は、include前に `UART1_BAUD_RATE` をdefineしてください。
 */
void uart_begin(void);

/**
 * @brief 単一byteを送信
 *
 * @param data
 */
void uart_write(uint8_t data);

/**
 * @brief 文字列を送信
 *
 * @param str
 * @note 終端 (`\0`) が来るまで送信し続けます。
 */
void uart_print(const char* const str);

/**
 * @brief 単一byteを16進数形式で送信
 *
 * @param value
 */
void uart_print_hex(uint8_t value);

/**
 * @brief 単一byteを受信
 *
 * @note 受信が完了するまでブロックします。受信できるデータがあるか確認する場合は `uart_available` を利用します。
 * @return uint8_t 受信した値
 */
uint8_t uart_read(void);

/**
 * @brief 受信バッファの状態を取得
 *
 * @return 受信できるデータがあるか
 */
bool uart_available(void);

#endif /* HARDWARE_UART1_H */
