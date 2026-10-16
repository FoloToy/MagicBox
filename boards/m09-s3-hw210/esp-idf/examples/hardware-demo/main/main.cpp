#include "bsp.h"
#include "bsp_motion.h"
#include <array>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
extern "C" void app_main() {
  ESP_ERROR_CHECK(bsp::begin());
  ESP_ERROR_CHECK(bsp::rgb(0, 8, 0));
  MagicBoxMotion motion;
  const bool motion_ready = motion.Begin();
  if (!motion_ready)
    ESP_LOGE("hardware-demo", "Motion sensor initialization failed");
  ESP_ERROR_CHECK(bsp::audio_begin());
  std::array<int16_t, 160> tone;
  for (unsigned i = 0; i < tone.size(); ++i)
    tone[i] = (i % 16 < 8) ? 500 : -500;
  for (int i = 0; i < 10; ++i) {
    size_t written;
    ESP_ERROR_CHECK(bsp::audio_write(tone.data(), tone.size(), &written, 1000));
    ESP_ERROR_CHECK(written == tone.size() ? ESP_OK : ESP_FAIL);
  }
  bsp::audio_end();
  for (;;) {
    if (motion_ready)
      motion.Poll();
    int raw;
    bool external;
    ESP_ERROR_CHECK(bsp::knob_raw(&raw));
    ESP_ERROR_CHECK(bsp::external_mode_pressed(&external));
    ESP_LOGI("hardware-demo", "M09 knob=%d mode=%d voice=%d external_mode=%d",
             raw, bsp::button_pressed(0), bsp::button_pressed(1), external);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
