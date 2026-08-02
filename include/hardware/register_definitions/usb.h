#ifndef HARDWARE_REGISTER_DEFINITIONS_USB_H
#define HARDWARE_REGISTER_DEFINITIONS_USB_H

#include "hardware/intellisense_mock.h"

/**
 * @brief
 *      USB Type-C configuration-channel control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Controls the internal pull-up and pull-down resistors connected to
 *      the USB Type-C configuration-channel and VBUS-detection pins.
 *
 *      Bit assignments:
 *      - Bit 7 `bVBUS2_PD_EN`: Enable the internal 10 kOhm VBUS2 pull-down
 *      - Bit 6 `bUCC2_PD_EN`: Enable the internal 5.1 kOhm UCC2 pull-down
 *      - Bit 5 `bUCC2_PU1_EN`: UCC2 pull-up selection, high bit
 *      - Bit 4 `bUCC2_PU0_EN`: UCC2 pull-up selection, low bit
 *      - Bit 3 `bVBUS1_PD_EN`: Enable the internal 10 kOhm VBUS1 pull-down
 *      - Bit 2 `bUCC1_PD_EN`: Enable the internal 5.1 kOhm UCC1 pull-down
 *      - Bit 1 `bUCC1_PU1_EN`: UCC1 pull-up selection, high bit
 *      - Bit 0 `bUCC1_PU0_EN`: UCC1 pull-up selection, low bit
 *
 *      The `bUCCn_PU1_EN:bUCCn_PU0_EN` combination selects the pull-up
 *      resistor for each UCC pin:
 *
 *      - `00`: Pull-up disabled
 *      - `01`: 56 kOhm pull-up, default USB current advertisement
 *      - `10`: 22 kOhm pull-up, 1.5 A current advertisement
 *      - `11`: 10 kOhm pull-up, 3 A current advertisement
 *
 * @note
 *      The Type-C pull-up and pull-down resistors operate independently
 *      from the GPIO direction and pull-up settings in `P1_DIR_PU`.
 *
 * @note
 *      When a UCC pin is used for Type-C detection, configure the
 *      corresponding GPIO as a high-impedance input and disable its normal
 *      GPIO pull-up.
 *
 * @note
 *      CH552E exposes UCC1 on P1.4 and UCC2 on P1.5. Its package does not
 *      expose VBUS1 or VBUS2.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0x91) USB_C_CTRL;

/**
 * @brief
 *      USB interrupt flags and current-transfer status register
 *      (Reset value: `0x20`)
 *
 * @details
 *      Contains USB interrupt flags in bits 4 through 0 and immediate
 *      status information for the current USB transaction in bits 7
 *      through 5.
 *
 *      Bit assignments:
 *      - Bit 7 `U_IS_NAK`: Current transaction received a NAK response
 *      - Bit 6 `U_TOG_OK`: DATA0/DATA1 synchronization matched
 *      - Bit 5 `U_SIE_FREE`: USB protocol processor is idle
 *      - Bit 4 `UIF_FIFO_OV`: USB FIFO overflow interrupt flag
 *      - Bit 3: Reserved
 *      - Bit 2 `UIF_SUSPEND`: Suspend or wake-up event interrupt flag
 *      - Bit 1 `UIF_TRANSFER`: USB transaction-complete interrupt flag
 *      - Bit 0 `UIF_BUS_RST`: USB bus-reset event interrupt flag
 *
 *      `U_IS_NAK`, `U_TOG_OK`, and `U_SIE_FREE` are read-only status bits.
 *
 *      Each writable interrupt flag can be cleared by writing either `0`
 *      directly to that bit or `1` to the corresponding bit.
 *
 * @warning
 *      Avoid an unintentional read-modify-write when clearing flags,
 *      because another pending interrupt flag may be cleared at the same
 *      time. Prefer writing the intended flag mask directly.
 *
 * @note
 *      This register is bit-addressable.
 */
__sfr __at(0xD8) USB_INT_FG;

/** Current USB transaction received a NAK response. Read-only. */
__sbit __at(0xDF) U_IS_NAK;

/** Current USB transaction has a matching DATA0/DATA1 toggle. Read-only. */
__sbit __at(0xDE) U_TOG_OK;

/** USB serial interface engine is idle. Read-only. */
__sbit __at(0xDD) U_SIE_FREE;

/** USB FIFO overflow interrupt flag. */
__sbit __at(0xDC) UIF_FIFO_OV;

/** USB suspend or wake-up event interrupt flag. */
__sbit __at(0xDA) UIF_SUSPEND;

/** USB transaction-complete interrupt flag. */
__sbit __at(0xD9) UIF_TRANSFER;

/** USB bus-reset event interrupt flag. */
__sbit __at(0xD8) UIF_BUS_RST;

/**
 * @brief
 *      USB transaction status register
 *      (Reset value: `00xx xxxx`)
 *
 * @details
 *      Reports the result and target of the most recently completed USB
 *      transaction.
 *
 *      Bit assignments:
 *      - Bit 7 `bUIS_IS_NAK`: The transaction received a NAK response
 *      - Bit 6 `bUIS_TOG_OK`: DATA0/DATA1 synchronization matched
 *      - Bit 5 `bUIS_TOKEN1`: Token PID, high bit
 *      - Bit 4 `bUIS_TOKEN0`: Token PID, low bit
 *      - Bits 3:0 `MASK_UIS_ENDP`: Endpoint number
 *
 *      Token encoding in bits 5:4:
 *
 *      - `00`: OUT
 *      - `01`: SOF
 *      - `10`: IN
 *      - `11`: SETUP
 *
 *      Bits 3:0 contain the endpoint number associated with the
 *      transaction. CH552 implements endpoints 0 through 4.
 *
 * @note
 *      This register is read-only.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xD9) USB_INT_ST;

#define UIS_TOKEN_OUT 0
#define UIS_TOKEN_SOF 1
#define UIS_TOKEN_IN 2
#define UIS_TOKEN_SETUP 3

/**
 * @brief
 *      USB miscellaneous status register
 *      (Reset value: `xx10 1000`)
 *
 * @details
 *      Reports the current state of the USB serial interface engine,
 *      receive FIFO, bus-reset detection, and suspend detection.
 *
 *      Bit assignments:
 *      - Bits 7:6: Reserved
 *      - Bit 5 `bUMS_SIE_FREE`: USB protocol processor is idle
 *      - Bit 4 `bUMS_R_FIFO_RDY`: Receive FIFO contains data
 *      - Bit 3 `bUMS_BUS_RESET`: USB bus reset is currently in progress
 *      - Bit 2 `bUMS_SUSPEND`: USB bus is currently suspended
 *      - Bits 1:0: Reserved
 *
 *      `bUMS_SUSPEND` becomes set after no USB activity has been detected
 *      for the controller-defined suspend interval. It is cleared when USB
 *      bus activity resumes.
 *
 * @note
 *      This register is read-only.
 *
 * @note
 *      `bUMS_SIE_FREE` represents the same SIE idle condition as
 *      `USB_INT_FG.U_SIE_FREE`.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xDA) USB_MIS_ST;

/**
 * @brief
 *      USB receive-length register
 *
 * @details
 *      Contains the number of bytes received by the endpoint involved in
 *      the most recently completed OUT or SETUP transaction.
 *
 *      The received payload itself is stored in the endpoint's xRAM DMA
 *      buffer. This register contains only its length.
 *
 * @note
 *      This register is read-only.
 *
 * @note
 *      The maximum packet size supported by CH552 endpoints is 64 bytes.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xDB) USB_RX_LEN;

/**
 * @brief
 *      USB event interrupt-enable register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Enables the individual USB events that can generate the shared USB
 *      interrupt request.
 *
 *      Bit assignments:
 *      - Bit 7 `bUIE_DEV_SOF`: Enable received-SOF interrupt
 *      - Bit 6 `bUIE_DEV_NAK`: Enable received-NAK interrupt
 *      - Bit 5: Reserved
 *      - Bit 4 `bUIE_FIFO_OV`: Enable FIFO-overflow interrupt
 *      - Bit 3: Reserved
 *      - Bit 2 `bUIE_SUSPEND`: Enable suspend/wake-up event interrupt
 *      - Bit 1 `bUIE_TRANSFER`: Enable transaction-complete interrupt
 *      - Bit 0 `bUIE_BUS_RST`: Enable bus-reset event interrupt
 *
 * @note
 *      Enabling an event here is not sufficient by itself. The USB
 *      interrupt must also be enabled through `IE_EX.IE_USB`, and global
 *      interrupts must be enabled through `IE.EA`.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xE1) USB_INT_EN;

/**
 * @brief
 *      USB controller configuration register
 *      (Reset value: `0x06`)
 *
 * @details
 *      Controls USB speed, device connection, automatic busy handling,
 *      SIE reset, FIFO clearing, and USB DMA.
 *
 *      Bit assignments:
 *      - Bit 7: Reserved
 *      - Bit 6 `bUC_LOW_SPEED`: Select Full-Speed or Low-Speed operation
 *      - Bit 5 `bUC_DEV_PU_EN` / `bUC_SYS_CTRL1`:
 *        USB device and internal pull-up control
 *      - Bit 4 `bUC_SYS_CTRL0`: USB system-control low bit
 *      - Bit 3 `bUC_INT_BUSY`: Automatically respond with NAK while the
 *        transfer-complete flag remains pending
 *      - Bit 2 `bUC_RESET_SIE`: Reset the USB serial interface engine and
 *        most USB control registers
 *      - Bit 1 `bUC_CLR_ALL`: Clear USB interrupt flags and FIFO contents
 *      - Bit 0 `bUC_DMA_EN`: Enable USB DMA
 *
 *      Speed selection:
 *
 *      - `bUC_LOW_SPEED = 0`: Full-Speed, 12 Mbit/s
 *      - `bUC_LOW_SPEED = 1`: Low-Speed, 1.5 Mbit/s
 *
 *      USB system-control selection using bits 5:4:
 *
 *      - `00`: Disable USB device function and internal pull-up
 *      - `01`: Enable USB device function without the internal pull-up
 *      - `1x`: Enable USB device function and the internal 1.5 kOhm pull-up
 *
 *      Setting `bUC_INT_BUSY` prevents a newly received transaction from
 *      overwriting state that has not yet been handled. Until software
 *      clears `UIF_TRANSFER`, the controller automatically returns NAK.
 *
 * @note
 *      `bUC_RESET_SIE` and `bUC_CLR_ALL` are set after reset. Software must
 *      clear them before normal USB operation.
 *
 * @warning
 *      Enabling `bUC_DEV_PU_EN` electrically announces the device's
 *      presence to the host. Configure the endpoint buffers and controller
 *      state before enabling the internal pull-up.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xE2) USB_CTRL;

/** Select Full-Speed or Low-Speed operation */
#define bUC_LOW_SPEED (1u << 6)

/** USB device and internal pull-up control */
#define bUC_DEV_PU_EN (1u << 5)

/** USB device and internal pull-up control */
#define bUC_SYS_CTRL1 (1u << 5)

/** USB system-control low bit */
#define bUC_SYS_CTRL0 (1u << 4)

/** Automatically respond with NAK while the transfer-complete flag remains pending */
#define bUC_INT_BUSY (1u << 3)

/** Reset the USB serial interface engine and most USB control registers */
#define bUC_RESET_SIE (1u << 2)

/** Clear USB interrupt flags and FIFO contents */
#define bUC_CLR_ALL (1u << 1)

/** Enable USB DMA */
#define bUC_DMA_EN (1u << 0)

/**
 * @brief
 *      USB device-address register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Stores the active 7-bit USB device address and one software-defined
 *      general-purpose flag.
 *
 *      Bit assignments:
 *      - Bit 7 `bUDA_GP_BIT`: User-defined general-purpose flag
 *      - Bits 6:0 `MASK_USB_ADDR`: USB device address
 *
 *      The address range is 0 through 127. Address 0 is used after USB bus
 *      reset and during the initial enumeration sequence.
 *
 * @warning
 *      For a standard `SET_ADDRESS` request, do not change this register
 *      immediately after receiving the SETUP packet. Apply the new address
 *      only after the status-stage IN transaction has completed.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xE3) USB_DEV_AD;

#define bUDA_GP_BIT (1u << 7)
#define MASK_USB_ADDR 0b01111111

#endif /* HARDWARE_REGISTER_DEFINITIONS_USB_H */
