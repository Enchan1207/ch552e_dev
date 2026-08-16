#include "hardware/usb_setup_fifo.h"

typedef struct {
    volatile uint8_t head;
    volatile uint8_t tail;
    usb_setup_packet_t entries[USB_SETUP_FIFO_SIZE];
} usb_setup_fifo_t;

static __xdata usb_setup_fifo_t fifo;

void usb_setup_fifo_init(void) {
    fifo.head = 0;
    fifo.tail = 0;
}

bool usb_setup_fifo_push_isr(const usb_setup_packet_t __xdata* packet) {
    uint8_t head = fifo.head;
    uint8_t next_head = (head + 1) & (USB_SETUP_FIFO_SIZE - 1);

    if (next_head == fifo.tail) {
        return false;
    }

    fifo.entries[head] = *packet;
    fifo.head = next_head;
    return true;
}

bool usb_setup_fifo_pop(usb_setup_packet_t* packet) {
    uint8_t tail = fifo.tail;

    if (tail == fifo.head) {
        return false;
    }

    *packet = fifo.entries[tail];
    fifo.tail = (tail + 1) & (USB_SETUP_FIFO_SIZE - 1);
    return true;
}
