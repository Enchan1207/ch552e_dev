#ifndef HARDWARE_USB_H
#define HARDWARE_USB_H

#include <ch552e/interrupt.h>

/** (デバッグ用) 最後にUSB割込みがかかった時のフラグ */
extern volatile uint8_t usb_debug_interrupt_flags;

/** (デバッグ用) 最後にUSB割込み(UIF_TRANSFER)がかかった時の状態 */
extern volatile uint8_t usb_debug_interrupt_status;

/** (デバッグ用) 最後にUSB割込み(UIF_TRANSFER)がかかった時のデータ長 */
extern volatile uint8_t usb_debug_interrupt_rx_length;

ISR(INT_NO_USB);

void usb_init(void);

#endif /* HARDWARE_USB_H */
