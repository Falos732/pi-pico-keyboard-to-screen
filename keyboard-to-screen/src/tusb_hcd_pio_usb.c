#include "tusb.h"
#include "pio_usb.h"
#include "pio_usb_ll.h"

// Root port unique
#define RHPORT 0

const pio_usb_configuration_t pio_usb_host_config = {
    .pin_dp        = PIO_USB_DP_PIN_DEFAULT,  // GPIO D+ (0 par défaut)
    .pio_tx_num    = PIO_USB_TX_DEFAULT,      // PIO utilisé pour TX
    .sm_tx         = PIO_SM_USB_TX_DEFAULT,   // state machine TX
    .tx_ch         = PIO_USB_DMA_TX_DEFAULT,  // canal DMA TX
    .pio_rx_num    = PIO_USB_RX_DEFAULT,      // PIO utilisé pour RX
    .sm_rx         = PIO_SM_USB_RX_DEFAULT,   // state machine RX
    .sm_eop        = PIO_SM_USB_EOP_DEFAULT,  // state machine EOP
    .alarm_pool    = NULL,                    // NULL => PIO-USB créera son propre alarm_pool
    .debug_pin_rx  = PIO_USB_DEBUG_PIN_NONE,  // pas de pin debug RX
    .debug_pin_eop = PIO_USB_DEBUG_PIN_NONE,  // pas de pin debug EOP
    .skip_alarm_pool = true,                  // true => tu dois appeler pio_usb_host_frame() toutes les 1ms
    .pinout        = PIO_USB_PINOUT_DPDM      // câblage DP/DM standard
};
bool hcd_init(uint8_t rhport) {
  (void) rhport;
  // config à définir dans ton projet
  pio_usb_host_init(&pio_usb_host_config);
  return true;
}

void hcd_int_enable(uint8_t rhport) { (void) rhport; }
void hcd_int_disable(uint8_t rhport) { (void) rhport; }

bool hcd_port_connect_status(uint8_t rhport) {
  (void) rhport;
  return PIO_USB_ROOT_PORT(RHPORT)->connected;
}

void hcd_port_reset(uint8_t rhport) {
  (void) rhport;
  pio_usb_host_port_reset_start(RHPORT);
}
void hcd_port_reset_end(uint8_t rhport) {
  (void) rhport;
  pio_usb_host_port_reset_end(RHPORT);
}

tusb_speed_t hcd_port_speed_get(uint8_t rhport) {
  (void) rhport;
  return PIO_USB_ROOT_PORT(RHPORT)->is_fullspeed ? TUSB_SPEED_FULL : TUSB_SPEED_LOW;
}

void hcd_device_close(uint8_t rhport, uint8_t dev_addr) {
  (void) rhport;
  pio_usb_host_close_device(RHPORT, dev_addr);
}

bool hcd_edpt_open(uint8_t rhport, uint8_t dev_addr, tusb_desc_endpoint_t const * desc_edpt) {
  (void) rhport;
  return pio_usb_host_endpoint_open(RHPORT, dev_addr, (uint8_t const*)desc_edpt, false);
}

bool hcd_edpt_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr, uint8_t * buffer, uint16_t buflen) {
  (void) rhport;
  return pio_usb_host_endpoint_transfer(RHPORT, dev_addr, ep_addr, buffer, buflen);
}

bool hcd_edpt_abort_xfer(uint8_t rhport, uint8_t dev_addr, uint8_t ep_addr) {
  (void) rhport;
  return pio_usb_host_endpoint_abort_transfer(RHPORT, dev_addr, ep_addr);
}

bool hcd_setup_send(uint8_t rhport, uint8_t dev_addr, uint8_t const setup[8], bool need_pre) {
  (void) rhport; (void) need_pre;
  return pio_usb_host_send_setup(RHPORT, dev_addr, setup);
}
