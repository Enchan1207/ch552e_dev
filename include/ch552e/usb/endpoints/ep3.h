#ifndef CH552E_USB_ENDPOINTS_EP3_H
#define CH552E_USB_ENDPOINTS_EP3_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      Endpoint 3 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP3_DMA_L` and `UEP3_DMA_H`.
 */
__sfr16 __at(0xE7E6) UEP3_DMA;

/**
 * @brief
 *      Endpoint 3 control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Same format as `UEP1_CTRL`, applied to endpoint 3.
 *
 * @note
 *      `bUEP_AUTO_TOG` is supported on endpoints 1, 2, and 3.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD6) UEP3_CTRL;

/**
 * @brief
 *      Endpoint 3 transmission-length register
 *      (Reset value: `0xxx xxxxb`)
 *
 * @details
 *      Specifies the number of bytes currently prepared in the endpoint 3 IN
 *      buffer for the next USB transmission.
 *
 * @note
 *      The maximum endpoint packet size is 64 bytes.
 */
__sfr __at(0xD7) UEP3_T_LEN;

#endif /* CH552E_USB_ENDPOINTS_EP3_H */
