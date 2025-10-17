#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// Common configuration
#define CFG_TUSB_MCU             OPT_MCU_RP2040
#define CFG_TUSB_OS              OPT_OS_PICO
#define CFG_TUSB_RHPORT0_MODE    (OPT_MODE_HOST | OPT_MODE_FULL_SPEED)

// Memory alignment (required by TinyUSB)
#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN       __attribute__ ((aligned(4)))

// Host configuration
#define CFG_TUH_HUB              0
#define CFG_TUH_DEVICE_MAX       1
#define CFG_TUH_ENDPOINT_MAX     8
#define CFG_TUH_HID              1

#endif // _TUSB_CONFIG_H_
