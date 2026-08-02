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

#endif /* HARDWARE_CH552E_H */
