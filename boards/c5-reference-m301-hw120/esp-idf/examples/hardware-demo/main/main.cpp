// Hardware diagnostic only; no radio, cloud, identity, OTA or product state machine.
#include "bsp_i2c.h"
#include "bsp_power.hpp"
#include "bsp_led.h"
#include "bsp_button.h"
#include "bsp_audio.h"
#include "bsp_knob.h"
#include "bsp_rtc.hpp"
#include "bsp_wakeup.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
static void button(bsp_button_t key, bsp_button_event_t event, void *) {
    ESP_LOGI("demo", "button=%d event=%d", key, event);
}
extern "C" void app_main() {
    ESP_ERROR_CHECK(bsp_i2c_init());
    ESP_ERROR_CHECK(bsp_power_init());
    ESP_ERROR_CHECK(bsp_led_init());
    ESP_ERROR_CHECK(bsp_knob_init());
    ESP_ERROR_CHECK(bsp_button_init(button, nullptr));
    ESP_ERROR_CHECK_WITHOUT_ABORT(bsp_rtc_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(bsp_wakeup_init());
    ESP_ERROR_CHECK(bsp_audio_init());
    ESP_ERROR_CHECK(bsp_audio_open(24000, 16, 1));
    ESP_ERROR_CHECK(bsp_audio_set_volume(20));
    bsp_i2c_scan();
    // One short, quiet local PCM tone. No bundled media.
    int16_t pcm[2400];
    for (int i = 0; i < 2400; ++i)
        pcm[i] = i % 48 < 24 ? 1000 : -1000;
    ESP_ERROR_CHECK(bsp_audio_write(pcm, sizeof pcm));
    while (true) {
        int raw;
        bsp_power_status_t power{};
        if (bsp_knob_read_raw(&raw) == ESP_OK && bsp_power_get_status(&power) == ESP_OK)
            ESP_LOGI("demo", "knob=%d battery=%d%%", raw, power.battery_percent);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
