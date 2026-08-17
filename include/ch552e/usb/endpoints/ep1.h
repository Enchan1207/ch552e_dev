#ifndef CH552E_USB_ENDPOINTS_EP1_H
#define CH552E_USB_ENDPOINTS_EP1_H

#include <sdcc/identifier.h>

/**
 * @brief
 *      Endpoint 1 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP1_DMA_L` and `UEP1_DMA_H`.
 */
__sfr16 __at(0xEFEE) UEP1_DMA;

/**
 * @brief
 *      Endpoint 1 control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Controls endpoint 1 DATA0/DATA1 synchronization, automatic toggle
 *      handling, and the handshake response returned for OUT and IN
 *      transactions.
 *
 *      Bit assignments:
 *      - Bit 7 `bUEP_R_TOG`: Expected DATA toggle for SETUP/OUT transactions
 *      - Bit 6 `bUEP_T_TOG`: DATA toggle to send for IN transactions
 *      - Bit 5: Reserved
 *      - Bit 4 `bUEP_AUTO_TOG`: Auto-toggle enable after successful transfer
 *      - Bits 3:2 `MASK_UEP_R_RES`: Response to SETUP/OUT transactions
 *      - Bits 1:0 `MASK_UEP_T_RES`: Response to IN transactions
 *
 *      Receive response encoding (`MASK_UEP_R_RES`):
 *      - `00`: ACK / ready to receive
 *      - `01`: No response / timeout
 *      - `10`: NAK / busy
 *      - `11`: STALL / error
 *
 *      Transmit response encoding (`MASK_UEP_T_RES`):
 *      - `00`: Send DATA0/DATA1 and expect ACK
 *      - `01`: Send DATA0/DATA1 without expecting a handshake
 *      - `10`: NAK / busy
 *      - `11`: STALL / error
 *
 * @note
 *      `bUEP_AUTO_TOG` is supported on endpoints 1, 2, and 3.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD2) UEP1_CTRL;

/**
 * @brief
 *      Endpoint 1 transmission-length register
 *      (Reset value: `0xxx xxxxb`)
 *
 * @details
 *      Specifies the number of bytes currently prepared in the endpoint 1 IN
 *      buffer for the next USB transmission.
 *
 * @note
 *      The maximum endpoint packet size is 64 bytes.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD3) UEP1_T_LEN;

#endif /* CH552E_USB_ENDPOINTS_EP1_H */
