#include <ch552e/interrupt.h>
#include <ch552e/io.h>
#include <stdint.h>

#include "bit_util.h"
#include "func.h"
#include "hardware/uart1.h"
#include "hardware/usb/usb.h"
#include "hardware/usb/usb_setup_fifo.h"

static const __code usb_device_descriptor_t device_descriptor = {
    .bLength = 18,
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x1234,
    .idProduct = 0x5678,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x00,
    .iProduct = 0x00,
    .iSerialNumber = 0x00,
    .bNumConfigurations = 0x01,
};

const __code usb_device_descriptor_t* usb_get_device_descriptor(void) {
    return &device_descriptor;
}

int main(void) {
    usb_setup_packet_t setup_packet;

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
