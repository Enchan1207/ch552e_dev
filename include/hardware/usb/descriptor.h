#ifndef HARDWARE_USB_DESCRIPTOR_H
#define HARDWARE_USB_DESCRIPTOR_H

#include <stdint.h>

// MARK: bDescriptorType

/** デバイスディスクリプタ */
#define USB_DESCRIPTOR_TYPE_DEVICE 0x01

/** コンフィギュレーションディスクリプタ */
#define USB_DESCRIPTOR_TYPE_CONFIGURATION 0x02

/** ストリングディスクリプタ */
#define USB_DESCRIPTOR_TYPE_STRING 0x03

/** インタフェースディスクリプタ */
#define USB_DESCRIPTOR_TYPE_INTERFACE 0x04

/** エンドポイントディスクリプタ */
#define USB_DESCRIPTOR_TYPE_ENDPOINT 0x05

// MARK: bEndpointAddress

/** エンドポイントディスクリプタ: アドレスmask */
#define USB_ENDPOINT_ADDRESS_MASK 0b00001111

/** エンドポイントディスクリプタ: 方向mask */
#define USB_ENDPOINT_DIRECTION_MASK 0b10000000

/** エンドポイントディスクリプタ: 入力 */
#define USB_ENDPOINT_DIRECTION_IN 0b10000000

/** エンドポイントディスクリプタ: 出力 */
#define USB_ENDPOINT_DIRECTION_OUT 0b00000000

// MARK: bMaxPower (configuration descriptor)

/** bMaxPower値の設定ヘルパ (単位: mA) */
#define USB_MAX_POWER(ma) ((ma) >> 1)

// MARK: bInterfaceClass (interface descriptor)

/** USBインタフェースクラス: USBオーディオ */
#define USB_INTERFACE_CLASS_AUDIO 0x01

/** USBインタフェースクラス: CDC (Communication Device Class) */
#define USB_INTERFACE_CLASS_CDC 0x02

/** USBインタフェースクラス: HID */
#define USB_INTERFACE_CLASS_HID 0x03

/** USBインタフェースクラス: CDCデータ */
#define USB_INTERFACE_CLASS_CDC_DATA 0x0A

/** USBインタフェースクラス: アプリケーション固有 */
#define USB_INTERFACE_CLASS_APPLICATION_SPECIFIC 0xFE

/** USBインタフェースクラス: ベンダ固有 */
#define USB_INTERFACE_CLASS_VENDOR_SPECIFIC 0xFF

// MARK: bmAttributes (endpoint descriptor)

/** エンドポイントディスクリプタ: 属性mask */
#define USB_ENDPOINT_DESCRIPTOR_ATTRIBUTE_MASK 0b00000011

/** エンドポイントディスクリプタ: 属性 (コントロール転送) */
#define USB_ENDPOINT_DESCRIPTOR_ATTRIBUTE_CONTROL 0b00000000

/** エンドポイントディスクリプタ: 属性 (アイソクロナス転送) */
#define USB_ENDPOINT_DESCRIPTOR_ATTRIBUTE_ISOCHRONOUS 0b00000001

/** エンドポイントディスクリプタ: 属性 (バルク転送) */
#define USB_ENDPOINT_DESCRIPTOR_ATTRIBUTE_BULK 0b00000010

/** エンドポイントディスクリプタ: 属性 (インタラプト転送) */
#define USB_ENDPOINT_DESCRIPTOR_ATTRIBUTE_INTERRUPT 0b00000011

// MARK: descriptor types

/** デバイスディスクリプタ */
typedef struct usb_device_descriptor_t {
    /** @note 0x12 固定 */
    uint8_t bLength;

    /** @note 0x01 (`USB_DESCRIPTOR_TYPE_DEVICE`) 固定 */
    uint8_t bDescriptorType;

    uint16_t bcdUSB;

    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;

    uint8_t bMaxPacketSize0;

    uint16_t idVendor;
    uint16_t idProduct;

    uint16_t bcdDevice;

    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;

    /** @note このライブラリは複数の Configuration Descriptor を想定しません。 */
    uint8_t bNumConfigurations;
} usb_device_descriptor_t;

/** コンフィギュレーションディスクリプタ */
typedef struct usb_configuration_descriptor_t {
    /** @note 0x09 固定 */
    uint8_t bLength;

    /** @note 0x02 (`USB_DESCRIPTOR_TYPE_CONFIGURATION`) 固定 */
    uint8_t bDescriptorType;

    uint16_t wTotalLength;

    uint8_t bNumInterface;

    uint8_t bConfigurationValue;

    uint8_t iConfiguration;

    uint8_t bmAttributes;
    uint8_t bMaxPower;
} usb_configuration_descriptor_t;

/** インタフェースディスクリプタ */
typedef struct usb_interface_descriptor_t {
    /** @note 0x09 固定 */
    uint8_t bLength;

    /** @note 0x04 (`USB_DESCRIPTOR_TYPE_INTERFACE`) 固定 */
    uint8_t bDescriptorType;

    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;

    uint8_t bNumEndpoints;

    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;

    uint8_t iInterface;
} usb_interface_descriptor_t;

/** エンドポイントディスクリプタ */
typedef struct usb_endpoint_descriptor_t {
    /** @note 0x07 固定 */
    uint8_t bLength;

    /** @note 0x05 (`USB_DESCRIPTOR_TYPE_ENDPOINT`) 固定 */
    uint8_t bDescriptorType;

    uint8_t bEndpointAddress;

    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} usb_endpoint_descriptor_t;

#endif /* HARDWARE_USB_DESCRIPTOR_H */
