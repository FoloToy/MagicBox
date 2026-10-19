#include "bsp_led.h"
#include "bsp_pins.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

static led_strip_handle_t s_strip;

esp_err_t bsp_led_init(void) {
    if (s_strip)
        return ESP_OK;
    const led_strip_config_t strip_config = {
        .strip_gpio_num = BSP_LED_DATA,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {.invert_out = false},
    };
    const led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 0,
        .flags = {.with_dma = false},
    };
    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_strip);
    return err == ESP_OK ? led_strip_clear(s_strip) : err;
}

esp_err_t bsp_led_set_rgb(uint8_t red, uint8_t green, uint8_t blue) {
    if (!s_strip)
        return ESP_ERR_INVALID_STATE;
    esp_err_t err = led_strip_set_pixel(s_strip, 0, red, green, blue);
    return err == ESP_OK ? led_strip_refresh(s_strip) : err;
}

esp_err_t bsp_led_off(void) { return s_strip ? led_strip_clear(s_strip) : ESP_ERR_INVALID_STATE; }

esp_err_t bsp_led_flash_white(uint32_t on_ms) {
    esp_err_t err = bsp_led_set_rgb(32, 32, 32);
    if (err != ESP_OK)
        return err;
    vTaskDelay(pdMS_TO_TICKS(on_ms));
    return bsp_led_off();
}
