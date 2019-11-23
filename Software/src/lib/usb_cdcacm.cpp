#include "lib/usb_cdcacm.h"

#include <cstdint>
#include <array>

#include "lib/rcc.h"
#include "lib/usb_structs.h"

namespace {
std::array<std::uint8_t, 128> _buffer = {};

usbd_request_return_codes cdcacm_control_request(usbd_device* usbd_dev,
                                                 struct usb_setup_data* req,
                                                 uint8_t** buf,
                                                 uint16_t* len,
                                                 void (** complete)(usbd_device* usbd_dev,
                                                                    struct usb_setup_data* req)) {
  (void) complete;
  (void) buf;
  (void) usbd_dev;

  switch (req->bRequest) {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
      /*
       * This Linux cdc_acm driver requires this to be implemented
       * even though it's optional in the CDC spec, and we don't
       * advertise it in the ACM functional descriptor.
       */
      return USBD_REQ_HANDLED;
    }
    case USB_CDC_REQ_SET_LINE_CODING:
      if (*len < sizeof(usb_cdc_line_coding)) {
        return USBD_REQ_NOTSUPP;
      }
      return USBD_REQ_HANDLED;
    default:
      // not handled
      break;
  }
  return USBD_REQ_NOTSUPP;
}

void cdcacm_data_rx_cb(usbd_device* usbd_dev, uint8_t ep) {
  (void) ep;

  std::array<char, 64> buf = {};
  std::uint16_t len = usbd_ep_read_packet(usbd_dev, 0x01, buf.data(), buf.size());

  if (len) {
    usbd_ep_write_packet(usbd_dev, 0x82, buf.data(), len);
    buf[len] = 0;
  }
}

void cdcacm_set_config(usbd_device* usbd_dev, uint16_t wValue) {
  (void) wValue;

  usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
  usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, nullptr);
  usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, nullptr);

  usbd_register_control_callback(
      usbd_dev,
      USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
      USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
      cdcacm_control_request);
}
}  // namespace

usb_cdcacm::usb_cdcacm() :
    _usb_dm_(GPIOA, GPIO11), _usb_dp_(GPIOA, GPIO12) {
  periph_setup();
  _usbd_dev_ = usbd_init(&st_usbfs_v1_usb_driver,
                         &_dev,
                         &_config,
                         _usb_strings.data(),
                         _usb_strings.size(),
                         _buffer.data(),
                         _buffer.size());
  usbd_register_set_config_callback(_usbd_dev_, &cdcacm_set_config);
}

void usb_cdcacm::periph_setup() {
  rcc::periph_clock_enable(RCC_USB);
  rcc::periph_clock_enable(RCC_GPIOA);

  _usb_dm_.setup(GPIO_MODE_AF, GPIO_PUPD_NONE);
  _usb_dp_.setup(GPIO_MODE_AF, GPIO_PUPD_NONE);
  _usb_dm_.set_af(GPIO_AF14);
  _usb_dp_.set_af(GPIO_AF14);
}

void usb_cdcacm::poll() {
  usbd_poll(_usbd_dev_);
}