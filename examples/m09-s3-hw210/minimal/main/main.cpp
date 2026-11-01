#include "bsp.h"
#include <esp_log.h>
extern "C" void app_main() {
  ESP_ERROR_CHECK(bsp::begin());
  ESP_LOGI("bsp",
           "M09 / ESP32-S3 / HW2.1.0 ready; no application services started");
}
