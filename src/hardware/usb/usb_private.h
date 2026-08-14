#ifndef HARDWARE_USB_PRIVATE_H
#define HARDWARE_USB_PRIVATE_H

#include <ch552e/io.h>
#include <stdint.h>

/** EP0のDMA転送先xRAMアドレス */
#define USB_EP0_DMA_ADDRESS 0x0000

extern __xdata __at(USB_EP0_DMA_ADDRESS)
/** xRAM上のEP0のバッファ */
uint8_t ep0_buffer[64];

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_packet_t;

// bmRequestType

#define MASK_MREQ_DIRECTION 0b10000000
#define MASK_MREQ_TYPE 0b01100000
#define MASK_MREQ_TARGET 0b00011111

#define MREQ_DIRECTION_H2D 0b00000000
#define MREQ_DIRECTION_D2H 0b10000000

#define MREQ_TYPE_STANDARD 0b00000000
#define MREQ_TYPE_CLASS 0b00100000
#define MREQ_TYPE_VENDOR 0b01000000

#define MREQ_TARGET_DEVICE 0b00000000
#define MREQ_TARGET_INTERFACE 0b00000001
#define MREQ_TARGET_ENDPOINT 0b00000010
#define MREQ_TARGET_OTHER 0b00000011

// bRequest

typedef enum usb_request_type_t {
    REQ_SET_ADDRESS = 0x05,
    REQ_GET_DESCRIPTOR = 0x06,

    /** なんのリクエストも受け取っていない */
    REQ_NONE = 0xFF,
} usb_request_type_t;

typedef enum usb_descriptor_type_t {
    DESCRIPTOR_TYPE_DEVICE = 0x01,
    DESCRIPTOR_TYPE_CONFIGURATION = 0x02,
} usb_descriptor_type_t;

#endif /* HARDWARE_USB_PRIVATE_H */
