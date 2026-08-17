#include <ch552e/io.h>
#include <ch552e/memory.h>

#include "endpoint_0_private.h"
#include "hardware/usb/usb.h"

__xdata __at(USB_EP0_DMA_ADDRESS)
uint8_t ep0_buffer[64];
