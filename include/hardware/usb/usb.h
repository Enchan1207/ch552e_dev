#ifndef HARDWARE_USB_H
#define HARDWARE_USB_H

#include <ch552e/interrupt.h>
#include <stdint.h>

#include "descriptor.h"
#include "hooks.h"
#include "usb_setup_fifo.h"

ISR(INT_NO_USB);

/**
 * @brief USBモジュールの初期化
 */
void usb_init(void);

#endif /* HARDWARE_USB_H */
