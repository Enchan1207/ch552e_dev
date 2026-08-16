#ifndef HARDWARE_USB_H
#define HARDWARE_USB_H

#include <ch552e/interrupt.h>

ISR(INT_NO_USB);

void usb_init(void);

#endif /* HARDWARE_USB_H */
