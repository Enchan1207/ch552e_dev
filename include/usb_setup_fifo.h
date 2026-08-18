#ifndef HARDWARE_USB_SETUP_FIFO_H
#define HARDWARE_USB_SETUP_FIFO_H

#include <sdcc/identifier.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef USB_SETUP_FIFO_SIZE
#define USB_SETUP_FIFO_SIZE 8
#endif

#define USB_SETUP_FIFO_ADDRESS 0x0040

#if (USB_SETUP_FIFO_SIZE & (USB_SETUP_FIFO_SIZE - 1)) != 0
#error "USB_SETUP_FIFO_SIZE must be a power of 2"
#endif

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_fifo_item_t;

void usb_setup_fifo_init(void);
bool usb_setup_fifo_push_isr(const usb_setup_fifo_item_t __xdata* packet);
bool usb_setup_fifo_pop(usb_setup_fifo_item_t* packet);

#endif /* HARDWARE_USB_SETUP_FIFO_H */
