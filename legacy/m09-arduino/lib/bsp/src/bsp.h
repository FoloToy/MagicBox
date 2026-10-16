#pragma once
#include <Arduino.h>
#include "esp_err.h"
// Hardware-only API. One application task owns lifecycle and audio calls.
namespace bsp {
void begin(); // GPIO, ADC, RGB; no network or persistent storage.
const char *board_name();
bool button_pressed(unsigned index); // physical inputs, no role/talk semantics.
uint16_t knob_raw();                 // ADC counts, no automatic volume policy.
void rgb(uint8_t red, uint8_t green, uint8_t blue);
esp_err_t audio_begin(); // two I2S controllers, mono 16 kHz PCM API.
esp_err_t audio_read(int16_t *pcm, size_t samples, size_t *read, uint32_t timeout_ms);
esp_err_t audio_write(const int16_t *pcm, size_t samples, size_t *written, uint32_t timeout_ms);
void audio_end();
} // namespace bsp
