#ifndef CH552E_USB_ENDPOINTS_EP1_H
#define CH552E_USB_ENDPOINTS_EP1_H

#include "sdcc/identifier.h"

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
 *      USB device physical-port control register
 *      (Reset value: `10xx 0000b`)
 *
 * @details
 *      Controls the USB D+ and D- physical-port interface and reports the
 *      current logic level present on the USB pins.
 *
 *      Bit assignments:
 *      - Bit 7 `bUD_PD_DIS`: Disable the internal pull-down resistors on UDP/UDM
 *      - Bit 6: Reserved
 *      - Bit 5 `bUD_DP_PIN`: Current UDP pin level, read-only
 *      - Bit 4 `bUD_DM_PIN`: Current UDM pin level, read-only
 *      - Bit 3: Reserved
 *      - Bit 2 `bUD_LOW_SPEED`: Select low-speed (`1.5 Mbps`) or full-speed (`12 Mbps`)
 *      - Bit 1 `bUD_GP_BIT`: Software-defined general-purpose flag
 *      - Bit 0 `bUD_PORT_EN`: Enable the USB physical port
 *
 * @note
 *      `bUD_PD_DIS` is independent of whether the USB physical interface
 *      is enabled. The internal pull-down resistors can also be used while
 *      UDP and UDM are operating as GPIO pins.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD1) UDEV_CTRL;

/** Disable the internal pull-down resistors on UDP/UDM */
#define bUD_PD_DIS (1u << 7)

/** Current UDP pin level, read-only */
#define bUD_DP_PIN (1u << 5)

/** Current UDM pin level, read-only */
#define bUD_DM_PIN (1u << 4)

/** Select low-speed (`1.5 Mbps`) or full-speed (`12 Mbps`) */
#define bUD_LOW_SPEED (1u << 2)

/** Software-defined general-purpose flag */
#define bUD_GP_BIT (1u << 1)

/** Enable the USB physical port */
#define bUD_PORT_EN (1u << 0)

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

/** Expected DATA toggle for SETUP/OUT transactions */
#define bUEP_R_TOG (1u << 7)

/** DATA toggle to send for IN transactions */
#define bUEP_T_TOG (1u << 6)

/** Auto-toggle enable after successful transfer */
#define bUEP_AUTO_TOG (1u << 4)

/** Response to SETUP/OUT transactions */
#define MASK_UEP_R_RES 0b00001100

/** Response to IN transactions */
#define MASK_UEP_T_RES 0b00000011

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
