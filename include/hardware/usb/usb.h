#ifndef HARDWARE_USB_H
#define HARDWARE_USB_H

#include <ch552e/interrupt.h>
#include <stdint.h>

#include "descriptor.h"

ISR(INT_NO_USB);

/**
 * @brief USBモジュールの初期化
 */
void usb_init(void);

/**
 * @brief デバイスディスクリプタ取得フック
 * @note この関数はユーザプログラムで実装する必要があります。
 *
 * @return const __code* デバイスディスクリプタへのポインタ
 */
extern const __code usb_device_descriptor_t* usb_get_device_descriptor(void);

#endif /* HARDWARE_USB_H */
