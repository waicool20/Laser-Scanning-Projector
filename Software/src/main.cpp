#include <cstdint>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "main.h"
#include "drivers/laser.h"
#include "drivers/laser_canvas.h"
#include "drivers/nav5.h"
#include "drivers/stepper_motor.h"
#include "drivers/st7735s.h"
#include "lib/rcc.h"
#include "lib/systick.h"
#include "lib/ui.h"
#include "views/view_main.h"

#include "images/smiley.h"

using namespace std::literals;

render rendering = render::NONE;

int main() {
  rcc::clock_setup_pll(rcc_hse8mhz_configs[RCC_CLOCK_HSE8_72MHZ]);
  systick::init();

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);

  laser laser{};
  stepper_motor xM{gpio(GPIOB, GPIO7), gpio(GPIOB, GPIO6), gpio(GPIOB, GPIO5)};
  stepper_motor yM{gpio(GPIOB, GPIO4), gpio(GPIOB, GPIO3), gpio(GPIOA, GPIO15)};
  gpio ldr = gpio(GPIOB, GPIO0);
  laser_canvas canvas{25600, 128, 72, laser, xM, yM, ldr};

  st7735s lcd{0, 0, 128, 160, st7735s::COLOR_MODE_18_BITS};
  nav5 nav5{
      gpio(GPIOC, GPIO15),
      gpio(GPIOC, GPIO13),
      gpio(GPIOC, GPIO14),
      gpio(GPIOB, GPIO8),
      gpio(GPIOB, GPIO9)
  };

  ui::init(&lcd, &nav5);
  view_main::show(true);

  canvas.home();

  while (true) {
    switch (rendering) {
      case render::BASIC_RECT:
        canvas.highlight_canvas_area();
        break;
      case render::BASIC_BITMAP:
        canvas.draw_frame();
        break;
      case render::BASIC_TUPLE:
        canvas.draw_tuples();
        break;
      case render::NONE:
      default:
        break;
    }
  }
}
