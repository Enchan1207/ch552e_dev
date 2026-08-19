#ifndef CH552E_USB_ENDPOINTS_EP2_H
#define CH552E_USB_ENDPOINTS_EP2_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      Endpoint 2 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP2_DMA_L` and `UEP2_DMA_H`.
 */
__sfr16 __at(0xE5E4) UEP2_DMA;

/**
 * @brief
 *      Endpoint 2 control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Same format as `UEP1_CTRL`, applied to endpoint 2.
 *
 * @note
 *      `bUEP_AUTO_TOG` is supported on endpoints 1, 2, and 3.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD4) UEP2_CTRL;

/**
 * @brief
 *      Endpoint 2 transmission-length register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Specifies the number of bytes currently prepared in the endpoint 2 IN
 *      buffer for the next USB transmission.
 *
 * @note
 *      Endpoint 2 resets to a defined zero length in the datasheet.
 *
 * @note
 *      The maximum endpoint packet size is 64 bytes.
 */
__sfr __at(0xD5) UEP2_T_LEN;

#endif /* CH552E_USB_ENDPOINTS_EP2_H */
