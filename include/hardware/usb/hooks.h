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

/**
 * @brief インタフェースディスクリプタ取得フック
 * @note
 *  この関数はユーザプログラムで実装する必要があります。
 *  指定されたインデックスのディスクリプタが存在しない場合は `NULL` を返してください。
 *
 * @param index インタフェースディスクリプタのインデックス
 * @return const __code* インタフェースディスクリプタへのポインタ
 */
extern const __code usb_interface_descriptor_t* usb_get_interface_descriptor(uint8_t index);

/**
 * @brief インタフェース従属ディスクリプタ取得フック
 * @note
 *  この関数はユーザプログラムで実装する必要があります。
 *  指定されたインデックスのディスクリプタが存在しない場合は `NULL` を返してください。
 *
 * @param if_index インタフェースディスクリプタのインデックス
 * @param index 従属ディスクリプタのインデックス
 * @param length 従属ディスクリプタのサイズ
 * @return uint8_t* 従属ディスクリプタへのポインタ
 */
extern const __code uint8_t* usb_get_interface_child_descriptor(uint8_t if_index, uint8_t index, uint8_t* length);

#endif /* HARDWARE_USB_HOOKS_H */
