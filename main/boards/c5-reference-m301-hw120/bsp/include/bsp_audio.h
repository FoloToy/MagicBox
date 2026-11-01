#pragma once

#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_audio_init(void);
esp_err_t bsp_audio_open(uint32_t sample_rate, uint8_t bits, uint8_t channels);
esp_err_t bsp_audio_close(void);
esp_err_t bsp_audio_write(const void *pcm, size_t bytes);
esp_err_t bsp_audio_read(void *pcm, size_t bytes);
esp_err_t bsp_audio_set_volume(uint8_t percent);
esp_err_t bsp_audio_set_input_gain(float db);

#ifdef __cplusplus
}
#endif
