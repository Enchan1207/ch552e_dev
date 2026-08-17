#ifndef HARDWARE_USB_H
#define HARDWARE_USB_H

#include <ch552e/interrupt.h>
#include <stdint.h>

ISR(INT_NO_USB);

// MARK: - bDescriptorType

/** デバイスディスクリプタ */
extern const __code usb_device_descriptor_t* device_descriptor;

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

    uint8_t bNumConfigurations;
} usb_device_descriptor_t;

/** USBモジュールの初期化 */
void usb_init(void);

/**
 * @brief デバイスディスクリプタを設定する
 *
 * @param descriptor
 */
void usb_set_device_descriptor(const __code usb_device_descriptor_t* descriptor);

#endif /* HARDWARE_USB_H */
