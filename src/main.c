#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stddef.h>
#include <stdint.h>

#include "bit_util.h"
#include "hardware/uart1.h"
#include "hardware/usb/usb.h"

#define USB_TEST_VENDOR_ID 0x1209
#define USB_TEST_PRODUCT_ID 0x0001

static const __code usb_device_descriptor_t device_descriptor = {
    .bLength = 18,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = USB_TEST_VENDOR_ID,
    .idProduct = USB_TEST_PRODUCT_ID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x00,
    .iProduct = 0x00,
    .iSerialNumber = 0x00,
    .bNumConfigurations = 0x01,
};

static const __code usb_configuration_descriptor_t configuration_descriptor = {
    .bLength = 0x09,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,

    .wTotalLength = 0x09 + 0x09,

    .bNumInterface = 0x01,
    .bConfigurationValue = 0x01,

    .iConfiguration = 0x00,
    .bmAttributes = 0x80,
    .bMaxPower = USB_MAX_POWER(100),
};

static const __code usb_interface_descriptor_t interface_descriptor = {
    .bLength = 0x09,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,

    .bInterfaceNumber = 0x00,
    .bAlternateSetting = 0x00,

    .bNumEndpoints = 0x00,

    .bInterfaceClass = USB_INTERFACE_CLASS_VENDOR_SPECIFIC,
    .bInterfaceSubClass = 0x00,
    .bInterfaceProtocol = 0x00,

    .iInterface = 0,
};

usb_device_descriptor_ptr usb_get_device_descriptor(void) {
    return &device_descriptor;
}

usb_configuration_descriptor_ptr usb_get_configuration_descriptor(void) {
    return &configuration_descriptor;
}

usb_interface_descriptor_ptr usb_get_interface_descriptor(uint8_t index) {
    if (index > 0) {
        return NULL;
    }

    return &interface_descriptor;
}

const __code uint8_t* usb_get_interface_child_descriptor(uint8_t if_index, uint8_t index, uint8_t* length) {
    // NOTE: 従属ディスクリプタなし
    (void)if_index;
    (void)index;
    (void)length;

    return NULL;
}

int main(void) {
    usb_setup_fifo_item_t setup_packet;

    // クロック設定 (内蔵オシレータ, 6MHz)
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    CLOCK_CFG = 0b10000011;
    SAFE_MOD = 0x00;

    // P1.4 (インジケータ) を出力にする
    P1_4 = 0;
    reset(P1_MOD_OC, 4);
    set(P1_DIR_PU, 4);

    uart_begin();

    usb_setup_fifo_init();

    usb_init();

    irq_enable();

    uart_print("Initialized.\r\n");

    while (1) {
        while (usb_setup_fifo_pop(&setup_packet)) {
            uart_print("SETUP bmRequestType:0x");
            uart_print_hex(setup_packet.bmRequestType);
            uart_print(" bRequest:0x");
            uart_print_hex(setup_packet.bRequest);
            uart_print(" wValue:0x");
            uart_print_hex((uint8_t)(setup_packet.wValue >> 8));
            uart_print_hex((uint8_t)setup_packet.wValue);
            uart_print(" wIndex:0x");
            uart_print_hex((uint8_t)(setup_packet.wIndex >> 8));
            uart_print_hex((uint8_t)setup_packet.wIndex);
            uart_print(" wLength:0x");
            uart_print_hex((uint8_t)(setup_packet.wLength >> 8));
            uart_print_hex((uint8_t)setup_packet.wLength);
            uart_print("\r\n");
        }
    }
}
