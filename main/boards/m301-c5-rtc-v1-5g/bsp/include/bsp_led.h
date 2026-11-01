#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_led_init(void);
esp_err_t bsp_led_set_rgb(uint8_t red, uint8_t green, uint8_t blue);
esp_err_t bsp_led_off(void);
esp_err_t bsp_led_flash_white(uint32_t on_ms);

#ifdef __cplusplus
}
#endif
