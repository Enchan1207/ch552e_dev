#ifndef CH552E_USB_ENDPOINTS_EP4_H
#define CH552E_USB_ENDPOINTS_EP4_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      Endpoint 4 control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Same layout as the generic `UEPn_CTRL` register, applied to endpoint 4.
 *
 * @note
 *      Endpoint 4 uses buffer memory carved out from the region starting at
 *      `UEP0_DMA`.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xDE) UEP4_CTRL;

/**
 * @brief
 *      Endpoint 4 transmission-length register
 *      (Reset value: `0xxx xxxxb`)
 *
 * @details
 *      Specifies the number of bytes currently prepared in the endpoint 4 IN
 *      buffer for the next USB transmission.
 */
__sfr __at(0xDF) UEP4_T_LEN;

#endif /* CH552E_USB_ENDPOINTS_EP4_H */
