// Minimal board bring-up. Run examples/hardware-demo for interactive diagnostics.
#include "bsp_i2c.h"
#include "bsp_power.hpp"
#include "bsp_led.h"
#include "esp_log.h"

extern "C" void app_main() {
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_power_init());
    ESP_ERROR_CHECK(bsp_led_init());
    ESP_LOGI("bsp", "MagicBox C5 M301 V1.2 BSP ready; no application services started");
}
