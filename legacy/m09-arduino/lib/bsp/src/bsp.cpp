#include "bsp.h"
#include "bsp_pins.h"
#include <FastLED.h>
#include "driver/i2s.h"
static CRGB pixel[1];
static bool ready = false, audio_ready = false;
namespace bsp {
void begin() {
    if (ready)
        return;
    for (unsigned i = 0; i < BSP_KEY_COUNT; i++) {
        pinMode(BSP_KEYS[i], INPUT_PULLUP);
    }
    pinMode(BSP_VOLUME, INPUT);
    analogReadResolution(12);
    FastLED.addLeds<WS2812B, BSP_RGB, GRB>(pixel, 1);
    rgb(0, 0, 0);
    ready = true;
}
const char *board_name() { return BSP_BOARD_NAME; }
bool button_pressed(unsigned index) {
    return index < BSP_KEY_COUNT && digitalRead(BSP_KEYS[index]) == LOW;
}
uint16_t knob_raw() { return analogRead(BSP_VOLUME); }
void rgb(uint8_t red, uint8_t green, uint8_t blue) {
    pixel[0] = CRGB(red, green, blue);
    FastLED.show();
}
esp_err_t audio_begin() {
    if (audio_ready)
        return ESP_OK;
    i2s_config_t cfg = {};
    cfg.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX);
    cfg.sample_rate = BSP_SAMPLE_RATE;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.intr_alloc_flags = 0;
    cfg.dma_buf_count = 4;
    cfg.dma_buf_len = 256;
    cfg.use_apll = false;
    auto err = i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr);
    if (err != ESP_OK)
        return err;
    i2s_pin_config_t pins = {};
    pins.mck_io_num = I2S_PIN_NO_CHANGE;
    pins.bck_io_num = BSP_MIC_BCLK;
    pins.ws_io_num = BSP_MIC_WS;
    pins.data_out_num = I2S_PIN_NO_CHANGE;
    pins.data_in_num = BSP_MIC_DATA;
    err = i2s_set_pin(I2S_NUM_0, &pins);
    if (err != ESP_OK) {
        i2s_driver_uninstall(I2S_NUM_0);
        return err;
    }
    cfg.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.tx_desc_auto_clear = true;
    err = i2s_driver_install(I2S_NUM_1, &cfg, 0, nullptr);
    if (err != ESP_OK) {
        i2s_driver_uninstall(I2S_NUM_0);
        return err;
    }
    pins.bck_io_num = BSP_SPEAKER_BCLK;
    pins.ws_io_num = BSP_SPEAKER_WS;
    pins.data_out_num = BSP_SPEAKER_DATA;
    pins.data_in_num = I2S_PIN_NO_CHANGE;
    err = i2s_set_pin(I2S_NUM_1, &pins);
    if (err != ESP_OK) {
        i2s_driver_uninstall(I2S_NUM_1);
        i2s_driver_uninstall(I2S_NUM_0);
        return err;
    }
    audio_ready = true;
    return ESP_OK;
}
esp_err_t audio_read(int16_t *pcm, size_t samples, size_t *read, uint32_t timeout_ms) {
    if (!pcm || !read || samples > 256)
        return ESP_ERR_INVALID_ARG;
    *read = 0;
    if (!audio_ready)
        return ESP_ERR_INVALID_STATE;
    int32_t raw[256];
    size_t bytes = 0;
    auto err =
        i2s_read(I2S_NUM_0, raw, samples * sizeof(int32_t), &bytes, pdMS_TO_TICKS(timeout_ms));
    *read = bytes / sizeof(int32_t);
    for (size_t i = 0; i < *read; i++)
        pcm[i] = static_cast<int16_t>(raw[i] >> 16);
    return err;
}
esp_err_t audio_write(const int16_t *pcm, size_t samples, size_t *written, uint32_t timeout_ms) {
    if (!pcm || !written || samples > SIZE_MAX / sizeof(int16_t))
        return ESP_ERR_INVALID_ARG;
    *written = 0;
    if (!audio_ready)
        return ESP_ERR_INVALID_STATE;
    size_t bytes = 0;
    auto err =
        i2s_write(I2S_NUM_1, pcm, samples * sizeof(int16_t), &bytes, pdMS_TO_TICKS(timeout_ms));
    *written = bytes / sizeof(int16_t);
    return err;
}
void audio_end() {
    if (!audio_ready)
        return;
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
    audio_ready = false;
}
} // namespace bsp
