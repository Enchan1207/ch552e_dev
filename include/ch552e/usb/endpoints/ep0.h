#ifndef CH552E_USB_ENDPOINTS_EP0_H
#define CH552E_USB_ENDPOINTS_EP0_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      Endpoint 0 and endpoint 4 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP0_DMA_L` and `UEP0_DMA_H`.
 *
 * @note
 *      Endpoint 0 always occupies the first 64 bytes from this base address.
 *      Endpoint 4 buffers, when enabled, are placed after it.
 */
__sfr16 __at(0xEDEC) UEP0_DMA;

/**
 * @brief
 *      Endpoint 0 control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Same layout as the generic `UEPn_CTRL` register, applied to the
 *      default control endpoint.
 *
 * @note
 *      Endpoint 0 is the default control endpoint and uses a shared 64-byte
 *      buffer for both receive and transmit traffic.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xDC) UEP0_CTRL;

/**
 * @brief
 *      Endpoint 0 transmission-length register
 *      (Reset value: `0xxx xxxxb`)
 *
 * @details
 *      Specifies the number of bytes currently prepared in the endpoint 0 IN
 *      buffer for the next control-transfer transmission.
 *
 * @note
 *      Endpoint 0 shares a single 64-byte buffer between OUT and IN traffic.
 */
__sfr __at(0xDD) UEP0_T_LEN;

#endif /* CH552E_USB_ENDPOINTS_EP0_H */
