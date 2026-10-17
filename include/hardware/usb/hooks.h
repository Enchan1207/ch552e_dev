#ifndef HARDWARE_USB_HOOKS_H
#define HARDWARE_USB_HOOKS_H

#include <sdcc/identifier.h>

#include "descriptor.h"

/** デバイスディスクリプタへのポインタ */
typedef const __code usb_device_descriptor_t* usb_device_descriptor_ptr;

/** コンフィギュレーションディスクリプタへのポインタ */
typedef const __code usb_configuration_descriptor_t* usb_configuration_descriptor_ptr;

/** Configuration 内の任意のディスクリプタへのポインタ */
typedef const __code uint8_t* usb_descriptor_ptr;

/**
 * @brief デバイスディスクリプタ取得フック
 * @note この関数はユーザプログラムで実装する必要があります。
 *
 * @return const __code* デバイスディスクリプタへのポインタ
 */
extern usb_device_descriptor_ptr usb_get_device_descriptor(void);

/**
 * @brief コンフィギュレーションディスクリプタ取得フック
 * @note この関数はユーザプログラムで実装する必要があります。
 *
 * @return const __code* コンフィギュレーションディスクリプタへのポインタ
 */
extern usb_configuration_descriptor_ptr usb_get_configuration_descriptor(void);

/**
 * @brief Configuration 直下のディスクリプタ取得フック
 * @note
 *  この関数はユーザプログラムで実装する必要があります。
 *  ディスクリプタは出力順に列挙し、存在しない場合は `NULL` を返してください。
 *  長さと種類は先頭の bLength / bDescriptorType から取得します。
 *  返すディスクリプタの bLength は 2 以上にしてください。
 *
 * @param entry_index Configuration 直下の列挙インデックス
 * @return ディスクリプタへのポインタ、または NULL
 */
extern usb_descriptor_ptr usb_get_configuration_entry(uint8_t entry_index);

/**
 * @brief Interface Entry に従属するディスクリプタ取得フック
 * @note
 *  この関数はユーザプログラムで実装する必要があります。
 *  Interface Entry ごとに出力順に列挙し、存在しない場合は `NULL` を返してください。
 *  長さと種類は先頭の bLength / bDescriptorType から取得します。
 *  返すディスクリプタの bLength は 2 以上にしてください。
 *
 * @param entry_index 親となる Interface Entry の列挙インデックス
 * @param child_index 従属ディスクリプタの列挙インデックス
 * @return ディスクリプタへのポインタ、または NULL
 */
extern usb_descriptor_ptr usb_get_configuration_entry_child(uint8_t entry_index, uint8_t child_index);

#endif /* HARDWARE_USB_HOOKS_H */
