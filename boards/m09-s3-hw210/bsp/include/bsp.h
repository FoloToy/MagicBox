#pragma once
#include "bsp_pins.h"
#include <cstddef>
#include <cstdint>
#include <esp_err.h>
namespace bsp {
// One application owner serializes init/lifecycle/I/O. No debounce or volume
// policy.
esp_err_t begin();
bool button_pressed(unsigned index);
esp_err_t external_mode_pressed(bool *pressed);
esp_err_t knob_raw(int *raw);
esp_err_t rgb(uint8_t red, uint8_t green, uint8_t blue);
// Optional audio owns both I2S controllers. 16-bit mono PCM, 16kHz.
// Read returns unamplified PCM; apply any gain in the application. Counts may
// be partial.
esp_err_t audio_begin();
void audio_end();
esp_err_t audio_read(int16_t *pcm, size_t samples, size_t *received,
                     uint32_t timeout_ms);
esp_err_t audio_write(const int16_t *pcm, size_t samples, size_t *written,
                      uint32_t timeout_ms);
} // namespace bsp
