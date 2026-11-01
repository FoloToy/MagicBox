#include "bsp.h"
#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <led_strip.h>
namespace {
adc_oneshot_unit_handle_t adc;
led_strip_handle_t led;
bool ready;
} // namespace
namespace bsp {
esp_err_t begin() {
  if (ready)
    return ESP_OK;
  gpio_config_t pins = {};
  pins.mode = GPIO_MODE_INPUT;
  pins.pull_up_en = GPIO_PULLUP_ENABLE;
  for (int pin : BSP_KEYS)
    pins.pin_bit_mask |= 1ULL << pin;
  pins.pin_bit_mask |= 1ULL << BSP_EXTERNAL_MODE;
  auto err = gpio_config(&pins);
  if (err != ESP_OK)
    return err;
  adc_oneshot_unit_init_cfg_t unit = {};
  unit.unit_id = ADC_UNIT_1;
  err = adc_oneshot_new_unit(&unit, &adc);
  if (err != ESP_OK)
    return err;
  adc_oneshot_chan_cfg_t channel = {};
  channel.atten = ADC_ATTEN_DB_12;
  channel.bitwidth = ADC_BITWIDTH_12;
  err = adc_oneshot_config_channel(adc, ADC_CHANNEL_7, &channel);
  if (err != ESP_OK) {
    adc_oneshot_del_unit(adc);
    adc = nullptr;
    return err;
  }
  led_strip_config_t config = {};
  config.strip_gpio_num = BSP_RGB;
  config.max_leds = 1;
  config.led_model = LED_MODEL_WS2812;
  config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
  led_strip_rmt_config_t rmt = {};
  rmt.clk_src = RMT_CLK_SRC_DEFAULT;
  rmt.resolution_hz = 10000000;
  err = led_strip_new_rmt_device(&config, &rmt, &led);
  if (err != ESP_OK) {
    adc_oneshot_del_unit(adc);
    adc = nullptr;
    return err;
  }
  err = led_strip_clear(led);
  if (err != ESP_OK) {
    led_strip_del(led);
    led = nullptr;
    adc_oneshot_del_unit(adc);
    adc = nullptr;
    return err;
  }
  ready = true;
  return ESP_OK;
}
bool button_pressed(unsigned index) {
  return ready && index < BSP_KEY_COUNT &&
         gpio_get_level(static_cast<gpio_num_t>(BSP_KEYS[index])) == 0;
}
esp_err_t external_mode_pressed(bool *pressed) {
  if (!pressed)
    return ESP_ERR_INVALID_ARG;
  if (!ready)
    return ESP_ERR_INVALID_STATE;
  *pressed = gpio_get_level(static_cast<gpio_num_t>(BSP_EXTERNAL_MODE)) == 0;
  return ESP_OK;
}
esp_err_t knob_raw(int *raw) {
  if (!raw)
    return ESP_ERR_INVALID_ARG;
  return ready ? adc_oneshot_read(adc, ADC_CHANNEL_7, raw)
               : ESP_ERR_INVALID_STATE;
}
esp_err_t rgb(uint8_t red, uint8_t green, uint8_t blue) {
  if (!ready)
    return ESP_ERR_INVALID_STATE;
  auto err = led_strip_set_pixel(led, 0, red, green, blue);
  return err == ESP_OK ? led_strip_refresh(led) : err;
}
} // namespace bsp
