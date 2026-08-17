#ifndef HARDWARE_USB_HOOKS_H
#define HARDWARE_USB_HOOKS_H

#include <sdcc/identifier.h>

#include "descriptor.h"

/**
 * @brief デバイスディスクリプタ取得フック
 * @note この関数はユーザプログラムで実装する必要があります。
 *
 * @return const __code* デバイスディスクリプタへのポインタ
 */
extern const __code usb_device_descriptor_t* usb_get_device_descriptor(void);

/**
 * @brief コンフィギュレーションディスクリプタ取得フック
 * @note この関数はユーザプログラムで実装する必要があります。
 *
 * @return const __code* コンフィギュレーションディスクリプタへのポインタ
 */
extern const __code usb_configuration_descriptor_t* usb_get_configuration_descriptor(void);

#endif /* HARDWARE_USB_HOOKS_H */
