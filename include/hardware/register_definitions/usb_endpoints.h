#ifndef HARDWARE_REGISTER_DEFINITIONS_USB_ENDPOINTS_H
#define HARDWARE_REGISTER_DEFINITIONS_USB_ENDPOINTS_H

#include "hardware/intellisense_mock.h"

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

/**
 * @brief
 *      Endpoint 1 and endpoint 4 mode-control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Enables endpoint 1 and endpoint 4 directions and selects the buffer
 *      layout used by their DMA regions.
 *
 *      Bit assignments:
 *      - Bit 7 `bUEP1_RX_EN`: Enable endpoint 1 OUT
 *      - Bit 6 `bUEP1_TX_EN`: Enable endpoint 1 IN
 *      - Bit 5: Reserved
 *      - Bit 4 `bUEP1_BUF_MOD`: Endpoint 1 single- or double-buffer mode
 *      - Bit 3 `bUEP4_RX_EN`: Enable endpoint 4 OUT
 *      - Bit 2 `bUEP4_TX_EN`: Enable endpoint 4 IN
 *      - Bits 1:0: Reserved
 *
 * @note
 *      Endpoint 1 can use single or double 64-byte buffers depending on
 *      `bUEP1_BUF_MOD`.
 *
 * @note
 *      Endpoint 4 buffer allocation is taken from the memory block beginning
 *      at `UEP0_DMA`: 64 bytes for endpoint 0, plus optional 64-byte RX and
 *      64-byte TX buffers for endpoint 4.
 */
__sfr __at(0xEA) UEP4_1_MOD;

/**
 * @brief
 *      Endpoint 2 and endpoint 3 mode-control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Enables endpoint 2 and endpoint 3 directions and selects whether each
 *      endpoint uses single or double 64-byte buffers.
 *
 *      Bit assignments:
 *      - Bit 7 `bUEP3_RX_EN`: Enable endpoint 3 OUT
 *      - Bit 6 `bUEP3_TX_EN`: Enable endpoint 3 IN
 *      - Bit 5: Reserved
 *      - Bit 4 `bUEP3_BUF_MOD`: Endpoint 3 single- or double-buffer mode
 *      - Bit 3 `bUEP2_RX_EN`: Enable endpoint 2 OUT
 *      - Bit 2 `bUEP2_TX_EN`: Enable endpoint 2 IN
 *      - Bit 1: Reserved
 *      - Bit 0 `bUEP2_BUF_MOD`: Endpoint 2 single- or double-buffer mode
 *
 * @note
 *      In double-buffer mode, the active 64-byte bank is selected by
 *      `bUEP_R_TOG` for OUT traffic and `bUEP_T_TOG` for IN traffic.
 */
__sfr __at(0xEB) UEP2_3_MOD;

/**
 * @brief
 *      Endpoint 0 and endpoint 4 DMA start-address high byte
 *      (Reset value: `0000 00xxb`)
 *
 * @details
 *      High byte of the shared DMA buffer base address used by endpoint 0 and
 *      endpoint 4. Only the low 2 bits of this register are significant.
 */
__sfr __at(0xED) UEP0_DMA_H;

/**
 * @brief
 *      Endpoint 0 and endpoint 4 DMA start-address low byte
 *      (Reset value: `xxxx xxxxb`)
 *
 * @details
 *      Low byte of the shared DMA buffer base address used by endpoint 0 and
 *      endpoint 4.
 */
__sfr __at(0xEC) UEP0_DMA_L;

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
__sfr16 __at(0xEC) UEP0_DMA;

/**
 * @brief
 *      Endpoint 1 DMA start-address high byte
 *      (Reset value: `0000 00xxb`)
 *
 * @details
 *      High byte of the endpoint 1 DMA buffer base address. Only the low
 *      2 bits are significant.
 */
__sfr __at(0xEF) UEP1_DMA_H;

/**
 * @brief
 *      Endpoint 1 DMA start-address low byte
 *      (Reset value: `xxxx xxxxb`)
 *
 * @details
 *      Low byte of the endpoint 1 DMA buffer base address.
 */
__sfr __at(0xEE) UEP1_DMA_L;

/**
 * @brief
 *      Endpoint 1 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP1_DMA_L` and `UEP1_DMA_H`.
 */
__sfr16 __at(0xEE) UEP1_DMA;

/**
 * @brief
 *      Endpoint 2 DMA start-address high byte
 *      (Reset value: `0000 00xxb`)
 *
 * @details
 *      High byte of the endpoint 2 DMA buffer base address. Only the low
 *      2 bits are significant.
 */
__sfr __at(0xE5) UEP2_DMA_H;

/**
 * @brief
 *      Endpoint 2 DMA start-address low byte
 *      (Reset value: `xxxx xxxxb`)
 *
 * @details
 *      Low byte of the endpoint 2 DMA buffer base address.
 */
__sfr __at(0xE4) UEP2_DMA_L;

/**
 * @brief
 *      Endpoint 2 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP2_DMA_L` and `UEP2_DMA_H`.
 */
__sfr16 __at(0xE4) UEP2_DMA;

/**
 * @brief
 *      Endpoint 3 DMA start-address high byte
 *      (Reset value: `0000 00xxb`)
 *
 * @details
 *      High byte of the endpoint 3 DMA buffer base address. Only the low
 *      2 bits are significant.
 */
__sfr __at(0xE7) UEP3_DMA_H;

/**
 * @brief
 *      Endpoint 3 DMA start-address low byte
 *      (Reset value: `xxxx xxxxb`)
 *
 * @details
 *      Low byte of the endpoint 3 DMA buffer base address.
 */
__sfr __at(0xE6) UEP3_DMA_L;

/**
 * @brief
 *      Endpoint 3 DMA start address
 *
 * @details
 *      Combined 16-bit alias for `UEP3_DMA_L` and `UEP3_DMA_H`.
 */
__sfr16 __at(0xE6) UEP3_DMA;

#endif /* HARDWARE_REGISTER_DEFINITIONS_USB_ENDPOINTS_H */
