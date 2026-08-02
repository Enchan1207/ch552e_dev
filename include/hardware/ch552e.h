#ifndef HARDWARE_CH552E_H
#define HARDWARE_CH552E_H

#include "hardware/intellisense_mock.h"

// MARK: - System configurations

/**
 * @brief
 *      Safe mode control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Some protected SFRs, such as `CLOCK_CFG`, can only be written
 *      while the device is in safe mode.
 *
 *      To enter safe mode, write `0x55` followed immediately by `0xAA`:
 *
 *      @code
 *      SAFE_MOD = 0x55;
 *      SAFE_MOD = 0xAA;
 *      @endcode
 *
 *      Safe mode remains active for approximately 13 to 23 system clock
 *      cycles. Protected SFR writes must be completed within this period.
 *      Safe mode is automatically terminated when the period expires.
 *
 *      Writing any other value to this register terminates safe mode
 *      immediately.
 *
 * @note
 *      This register is write-only. Reading address `0xA1` accesses
 *      `CHIP_ID` instead of `SAFE_MOD`.
 *
 * @warning
 *      Interrupts may consume the limited safe-mode period. When multiple
 *      instructions are required between entering safe mode and writing a
 *      protected SFR, execute the sequence atomically or with interrupts
 *      temporarily disabled.
 */
__sfr __at(0xA1) SAFE_MOD;

/**
 * @brief
 *      System clock configuration register
 *      (Reset value: `0x83`)
 * @warning This register only can be written in safe mode.
 */
__sfr __at(0xB9) CLOCK_CFG;

// MARK: - Interruption

#define INT_NO_INT0 0
#define INT_NO_TMR0 1
#define INT_NO_INT1 2
#define INT_NO_TMR1 3
#define INT_NO_UART0 4
#define INT_NO_TMR2 5
#define INT_NO_SPI0 6
#define INT_NO_TKEY 7
#define INT_NO_USB 8
#define INT_NO_ADC 9
#define INT_NO_UART1 10
#define INT_NO_PWMX 11
#define INT_NO_GPIO 12
#define INT_NO_WDOG 13

/**
 * @brief
 *      Extended interrupt priority control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Selects the priority level of the extended interrupt sources.
 *      A value of `1` assigns high priority; `0` assigns low priority.
 *
 *      Bit assignments:
 *      - Bit 7 `bIP_LEVEL`: Current interrupt nesting-level flag (read-only)
 *      - Bit 6 `bIP_GPIO`: GPIO interrupt priority
 *      - Bit 5 `bIP_PWMX`: PWM1/PWM2 interrupt priority
 *      - Bit 4 `bIP_UART1`: UART1 interrupt priority
 *      - Bit 3 `bIP_ADC`: ADC interrupt priority
 *      - Bit 2 `bIP_USB`: USB interrupt priority
 *      - Bit 1 `bIP_TKEY`: Touch-key timer interrupt priority
 *      - Bit 0 `bIP_SPI0`: SPI0 interrupt priority
 *
 *      Interrupt sources at the same priority level are serviced according
 *      to the fixed hardware priority order.
 *
 * @note
 *      `bIP_LEVEL` indicates the current interrupt nesting state:
 *      - `0`: No interrupt is active, or a second-level nested interrupt
 *             is active
 *      - `1`: A first-level interrupt is active
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xE9) IP_EX;

/**
 * @brief
 *      Extended interrupt enable register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Enables or disables each extended interrupt source.
 *      A value of `1` enables the corresponding interrupt request;
 *      `0` masks it.
 *
 *      Bit assignments:
 *      - Bit 7 `IE_WDOG`: Watchdog timer interrupt
 *      - Bit 6 `IE_GPIO`: GPIO interrupt
 *      - Bit 5 `IE_PWMX`: PWM1/PWM2 interrupt
 *      - Bit 4 `IE_UART1`: UART1 interrupt
 *      - Bit 3 `IE_ADC`: ADC interrupt
 *      - Bit 2 `IE_USB`: USB interrupt
 *      - Bit 1 `IE_TKEY`: Touch-key timer interrupt
 *      - Bit 0 `IE_SPI0`: SPI0 interrupt
 *
 * @note
 *      Enabling a source here is not sufficient by itself. Global
 *      interrupts must also be enabled through `IE.EA`, and the
 *      corresponding peripheral may require its own local interrupt-enable
 *      bits to be configured.
 *
 * @note
 *      This register is bit-addressable.
 */
__sfr __at(0xE8) IE_EX;
__sbit __at(0xE8) IE_SPI0;
__sbit __at(0xE9) IE_TKEY;
__sbit __at(0xEA) IE_USB;
__sbit __at(0xEB) IE_ADC;
__sbit __at(0xEC) IE_UART1;
__sbit __at(0xED) IE_PWMX;
__sbit __at(0xEE) IE_GPIO;
__sbit __at(0xEF) IE_WDOG;

/**
 * @brief
 *      GPIO interrupt source and trigger-mode control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Selects the GPIO pins that can generate the shared GPIO interrupt
 *      and configures level-sensitive or edge-sensitive operation.
 *
 *      Bit assignments:
 *      - Bit 7 `bIE_IO_EDGE`: GPIO interrupt trigger mode
 *      - Bit 6 `bIE_RXD1_LO`: UART1 RX pin interrupt enable
 *      - Bit 5 `bIE_P1_5_LO`: P1.5 interrupt enable
 *      - Bit 4 `bIE_P1_4_LO`: P1.4 interrupt enable
 *      - Bit 3 `bIE_P1_3_LO`: P1.3 interrupt enable
 *      - Bit 2 `bIE_RST_HI`: RST pin interrupt enable
 *      - Bit 1 `bIE_P3_1_LO`: P3.1 interrupt enable
 *      - Bit 0 `bIE_RXD0_LO`: UART0 RX pin interrupt enable
 *
 *      Trigger mode selected by `bIE_IO_EDGE`:
 *      - `0`: Level-sensitive mode
 *      - `1`: Edge-sensitive mode
 *
 *      Most sources are active-low in level mode and falling-edge active
 *      in edge mode. `RST` is active-high in level mode and rising-edge
 *      active in edge mode.
 *
 * @note
 *      The UART RX interrupt sources follow the pin mapping selected by
 *      `PIN_FUNC`.
 *
 * @note
 *      All enabled GPIO sources share the same GPIO interrupt vector.
 *      The GPIO interrupt itself must also be enabled through
 *      `IE_EX.IE_GPIO`.
 *
 * @warning
 *      In edge-sensitive mode, the GPIO interrupt-active flag cannot be
 *      cleared directly by software. It is cleared by reset, by switching
 *      to level-sensitive mode, or automatically when entering the GPIO
 *      interrupt service routine.
 *
 * @note
 *      This register is not bit-addressable and must be accessed as a byte.
 */
__sfr __at(0xC7) GPIO_IE;

/**
 * @brief
 *      Standard interrupt priority control register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Selects the priority level of the standard MCS-51-compatible
 *      interrupt sources.
 *
 *      For writable priority bits:
 *      - `1`: High priority
 *      - `0`: Low priority
 *
 *      Bit assignments:
 *      - Bit 7 `PH_FLAG`: High-priority interrupt active flag (read-only)
 *      - Bit 6 `PL_FLAG`: Low-priority interrupt active flag (read-only)
 *      - Bit 5 `PT2`: Timer 2 interrupt priority
 *      - Bit 4 `PS`: UART0 interrupt priority
 *      - Bit 3 `PT1`: Timer 1 interrupt priority
 *      - Bit 2 `PX1`: External interrupt 1 priority
 *      - Bit 1 `PT0`: Timer 0 interrupt priority
 *      - Bit 0 `PX0`: External interrupt 0 priority
 *
 *      `PH_FLAG` and `PL_FLAG` indicate the current interrupt state:
 *      - `00`: No interrupt is active
 *      - `01`: A low-priority interrupt is active
 *      - `10`: A high-priority interrupt is active
 *      - `11`: Unexpected state
 *
 *      Interrupt sources at the same priority level are serviced according
 *      to the fixed hardware priority order.
 *
 * @note
 *      This register is bit-addressable.
 */
__sfr __at(0xB8) IP;
__sbit __at(0xB8) PX0;
__sbit __at(0xB9) PT0;
__sbit __at(0xBA) PX1;
__sbit __at(0xBB) PT1;
__sbit __at(0xBC) PS;
__sbit __at(0xBD) PT2;
__sbit __at(0xBE) PL_FLAG;
__sbit __at(0xBF) PH_FLAG;

/**
 * @brief
 *      Standard interrupt enable register
 *      (Reset value: `0x00`)
 *
 * @details
 *      Controls global interrupt acceptance and enables the standard
 *      MCS-51-compatible interrupt sources.
 *
 *      Bit assignments:
 *      - Bit 7 `EA`: Global interrupt enable
 *      - Bit 6 `E_DIS`: Global interrupt disable
 *      - Bit 5 `ET2`: Timer 2 interrupt enable
 *      - Bit 4 `ES`: UART0 interrupt enable
 *      - Bit 3 `ET1`: Timer 1 interrupt enable
 *      - Bit 2 `EX1`: External interrupt 1 enable
 *      - Bit 1 `ET0`: Timer 0 interrupt enable
 *      - Bit 0 `EX0`: External interrupt 0 enable
 *
 *      Interrupt requests are accepted only when:
 *
 *      @code
 *      EA == 1 && E_DIS == 0
 *      @endcode
 *
 *      Setting `E_DIS` masks all interrupt requests regardless of `EA`.
 *      It can therefore be used for temporary global interrupt masking
 *      without changing the individual source-enable bits.
 *
 * @note
 *      `E_DIS` is commonly used to temporarily disable interrupts during
 *      Flash-ROM operations.
 *
 * @note
 *      Extended interrupt sources are enabled separately through `IE_EX`,
 *      but still require `EA == 1` and `E_DIS == 0`.
 *
 * @note
 *      This register is bit-addressable.
 */
__sfr __at(0xA8) IE;
__sbit __at(0xA8) EX0;
__sbit __at(0xA9) ET0;
__sbit __at(0xAA) EX1;
__sbit __at(0xAB) ET1;
__sbit __at(0xAC) ES;
__sbit __at(0xAD) ET2;
__sbit __at(0xAE) E_DIS;
__sbit __at(0xAF) EA;

// MARK: - GPIO

/**
 * @brief
 *      P1 port direction control and pull-up enable register
 *      (Reset value: `0xFF`)
 * @note
 *      Set direction control in push-pull output mode (0: in / 1: out)
 *
 *      Set pull-up register enable control in open-drain output mode (0: disabled / 1: enabled)
 */
__sfr __at(0x93) P1_DIR_PU;

/**
 * @brief
 *      P1 port output mode setting register
 *      (Reset value: `0xFF`)
 * @note 0: push-pull output / 1: open-drain output
 */
__sfr __at(0x92) P1_MOD_OC;

/**
 * @brief Port 1 Input/Output register
 * @note This register is bit-accessible with `P1_0` ~ `P1_7`.
 */
__sfr __at(0x90) P1;
__sbit __at(0x90) P1_0;
__sbit __at(0x91) P1_1;
__sbit __at(0x92) P1_2;
__sbit __at(0x93) P1_3;
__sbit __at(0x94) P1_4;
__sbit __at(0x95) P1_5;
__sbit __at(0x96) P1_6;
__sbit __at(0x97) P1_7;

// MARK: - UART1

/**
 * @brief
 *      UART1 control register
 *      (Reset value: `0x40`)
 * @note This register is bit-accessible with `U1RI`, `U1TI`, `U1REN`, `U1SMOD`, and `U1SM0`.
 */
__sfr __at(0xC0) SCON1;

/**
 * @brief UART1 receive interrupt flag
 * @note Set by hardware after a data byte is received successfully.
 *       This flag must be cleared by software.
 */
__sbit __at(0xC0) U1RI;

/**
 * @brief UART1 transmit interrupt flag
 * @note Set by hardware after a data byte has been transmitted.
 *       This flag must be cleared by software.
 */
__sbit __at(0xC1) U1TI;

/**
 * @brief UART1 receive enable control
 * @note 0: receive disabled / 1: receive enabled
 */
__sbit __at(0xC4) U1REN;

/**
 * @brief UART1 baud-rate mode selection
 * @note 0: slow mode (`Fsys / 32`) / 1: fast mode (`Fsys / 16`)
 */
__sbit __at(0xC5) U1SMOD;

/**
 * @brief UART1 operating mode selection
 * @note 0: 8-bit asynchronous communication / 1: 9-bit asynchronous communication
 */
__sbit __at(0xC7) U1SM0;

/**
 * @brief UART1 transmit and receive data register
 * @note
 *      The transmit and receive registers are physically separate.
 *      Writing accesses the transmit register; reading accesses the receive register.
 */
__sfr __at(0xC1) SBUF1;

/**
 * @brief UART1 baud-rate setting register
 * @note
 *      `U1SMOD = 0`: `SBAUD1 = 256 - Fsys / 32 / baud`
 *
 *      `U1SMOD = 1`: `SBAUD1 = 256 - Fsys / 16 / baud`
 */
__sfr __at(0xC2) SBAUD1;

// MARK: - USB

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

#endif /* HARDWARE_CH552E_H */
