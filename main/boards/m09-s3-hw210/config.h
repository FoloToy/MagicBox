#pragma once

#include <driver/gpio.h>
#include <bsp_pins.h>

// Shared M09 / ESP32-S3 / HW2.1.0 hardware facts; never duplicate Arduino's I2S driver here.
constexpr int kInputSampleRate = 16000;
constexpr int kOutputSampleRate = 24000;
constexpr gpio_num_t kModeButton = static_cast<gpio_num_t>(BSP_KEYS[0]);
constexpr gpio_num_t kExternalModeButton = static_cast<gpio_num_t>(BSP_EXTERNAL_MODE);
constexpr gpio_num_t kVoiceButton = static_cast<gpio_num_t>(BSP_KEYS[1]);
constexpr gpio_num_t kStatusLed = static_cast<gpio_num_t>(BSP_RGB);
static_assert(BSP_VOLUME == 8, "Update the ADC channel when the shared knob pin changes");
