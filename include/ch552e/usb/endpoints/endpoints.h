#ifndef CH552_USB_ENDPOINTS_H
#define CH552_USB_ENDPOINTS_H

#include "ep0.h"
#include "ep1.h"
#include "ep2.h"
#include "ep3.h"
#include "ep4.h"

/** Expected DATA toggle for SETUP/OUT transactions */
#define bUEP_R_TOG (1u << 7)

/** DATA toggle to send for IN transactions */
#define bUEP_T_TOG (1u << 6)

/** Auto-toggle enable after successful transfer */
#define bUEP_AUTO_TOG (1u << 4)

/** Response to SETUP/OUT transactions */
#define MASK_UEP_R_RES 0b00001100

#define UEP_R_RES_ACK 0b00000000
#define UEP_R_RES_NAK 0b00001000
#define UEP_R_RES_STALL 0b00001100

/** Response to IN transactions */
#define MASK_UEP_T_RES 0b00000011

#define UEP_T_RES_ACK 0b00000000
#define UEP_T_RES_NAK 0b00000010
#define UEP_T_RES_STALL 0b00000011

#endif /* CH552_USB_ENDPOINTS_H */
