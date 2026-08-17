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

// MARK: descriptor types

/** デバイスディスクリプタ */
typedef struct usb_device_descriptor_t {
    uint8_t bLength;
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
    uint8_t bLength;
    uint8_t bDescriptorType;

    uint16_t wTotalLength;

    uint8_t bNumInterface;

    uint8_t bConfigurationValue;

    uint8_t iConfiguration;

    uint8_t bmAttributes;
    uint8_t bMaxPower;
} usb_configuration_descriptor_t;

#endif /* HARDWARE_USB_DESCRIPTOR_H */
