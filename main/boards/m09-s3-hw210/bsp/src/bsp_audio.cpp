#include "bsp.h"
#include <algorithm>
#include <array>
#include <driver/i2s_std.h>
namespace {
i2s_chan_handle_t rx, tx;
bool ready;
esp_err_t channel(int id, bool input, i2s_chan_handle_t *handle) {
  i2s_chan_config_t config = I2S_CHANNEL_DEFAULT_CONFIG(id, I2S_ROLE_MASTER);
  config.dma_desc_num = 4;
  config.dma_frame_num = 256;
  config.auto_clear_after_cb = true;
  auto err = input ? i2s_new_channel(&config, nullptr, handle)
                   : i2s_new_channel(&config, handle, nullptr);
  if (err != ESP_OK)
    return err;
  i2s_std_config_t std = {};
  std.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(BSP_SAMPLE_RATE);
  std.slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT,
                                                     I2S_SLOT_MODE_MONO);
  std.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
  std.gpio_cfg.mclk = I2S_GPIO_UNUSED;
  std.gpio_cfg.bclk =
      static_cast<gpio_num_t>(input ? BSP_MIC_BCLK : BSP_SPEAKER_BCLK);
  std.gpio_cfg.ws =
      static_cast<gpio_num_t>(input ? BSP_MIC_WS : BSP_SPEAKER_WS);
  std.gpio_cfg.dout =
      input ? I2S_GPIO_UNUSED : static_cast<gpio_num_t>(BSP_SPEAKER_DATA);
  std.gpio_cfg.din =
      input ? static_cast<gpio_num_t>(BSP_MIC_DATA) : I2S_GPIO_UNUSED;
  err = i2s_channel_init_std_mode(*handle, &std);
  return err == ESP_OK ? i2s_channel_enable(*handle) : err;
}
void release(i2s_chan_handle_t &handle) {
  if (!handle)
    return;
  i2s_channel_disable(handle);
  i2s_del_channel(handle);
  handle = nullptr;
}
} // namespace
namespace bsp {
esp_err_t audio_begin() {
  if (ready)
    return ESP_OK;
  auto err = channel(I2S_NUM_0, true, &rx);
  if (err == ESP_OK)
    err = channel(I2S_NUM_1, false, &tx);
  if (err != ESP_OK) {
    release(rx);
    release(tx);
    return err;
  }
  ready = true;
  return ESP_OK;
}
void audio_end() {
  release(tx);
  release(rx);
  ready = false;
}
esp_err_t audio_read(int16_t *pcm, size_t samples, size_t *received,
                     uint32_t timeout_ms) {
  if (!pcm || !received || samples > 256)
    return ESP_ERR_INVALID_ARG;
  *received = 0;
  if (!ready)
    return ESP_ERR_INVALID_STATE;
  std::array<int32_t, 64> raw;
  while (*received < samples) {
    size_t bytes = 0;
    const size_t count = std::min(samples - *received, raw.size());
    auto err = i2s_channel_read(rx, raw.data(), count * sizeof(int32_t), &bytes,
                                timeout_ms);
    for (size_t i = 0; i < bytes / sizeof(int32_t); ++i)
      pcm[*received + i] = static_cast<int16_t>(raw[i] >> 16);
    *received += bytes / sizeof(int32_t);
    if (err != ESP_OK)
      return err;
    if (!bytes)
      return ESP_ERR_TIMEOUT;
  }
  return ESP_OK;
}
esp_err_t audio_write(const int16_t *pcm, size_t samples, size_t *written,
                      uint32_t timeout_ms) {
  if (!pcm || !written)
    return ESP_ERR_INVALID_ARG;
  *written = 0;
  if (!ready)
    return ESP_ERR_INVALID_STATE;
  std::array<int32_t, 64> raw;
  while (*written < samples) {
    const size_t count = std::min(samples - *written, raw.size());
    for (size_t i = 0; i < count; ++i)
      raw[i] = static_cast<int32_t>(pcm[*written + i]) * 32768;
    size_t bytes = 0;
    auto err = i2s_channel_write(tx, raw.data(), count * sizeof(int32_t),
                                 &bytes, timeout_ms);
    *written += bytes / sizeof(int32_t);
    if (err != ESP_OK)
      return err;
    if (!bytes)
      return ESP_ERR_TIMEOUT;
  }
  return ESP_OK;
}
} // namespace bsp
