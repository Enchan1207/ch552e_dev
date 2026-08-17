#include "hardware/usb.h"
#include "usb_private.h"

const __code usb_device_descriptor_t* device_descriptor;

void usb_set_device_descriptor(const __code usb_device_descriptor_t* descriptor) {
    device_descriptor = descriptor;
}
