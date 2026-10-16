#include "bsp_audio.h"
#include "bsp_i2c.h"
#include "bsp_pins.h"

#include "driver/i2s_std.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "es8311_codec.h"
#include "esp_log.h"

static const char *TAG = "bsp_audio";
static esp_codec_dev_handle_t s_input_device;
static esp_codec_dev_handle_t s_output_device;
static i2s_chan_handle_t s_tx;
static i2s_chan_handle_t s_rx;
static uint32_t s_rate;
static uint8_t s_bits;
static uint8_t s_channels;
static bool s_open;

static esp_err_t init_i2s() {
    const i2s_chan_config_t channel = {
        .id = BSP_I2S_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 6,
        .dma_frame_num = 240,
        .auto_clear_after_cb = true,
        .auto_clear_before_cb = false,
        .intr_priority = 0,
    };
    esp_err_t err = i2s_new_channel(&channel, &s_tx, &s_rx);
    if (err != ESP_OK)
        return err;

    const i2s_std_config_t standard = {
        .clk_cfg =
            {
                .sample_rate_hz = BSP_AUDIO_RATE,
                .clk_src = I2S_CLK_SRC_DEFAULT,
                .ext_clk_freq_hz = 0,
                .mclk_multiple = I2S_MCLK_MULTIPLE_256,
            },
        .slot_cfg =
            {
                .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
                .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
                .slot_mode = I2S_SLOT_MODE_STEREO,
                .slot_mask = I2S_STD_SLOT_BOTH,
                .ws_width = I2S_DATA_BIT_WIDTH_16BIT,
                .ws_pol = false,
                .bit_shift = true,
                .left_align = true,
                .big_endian = false,
                .bit_order_lsb = false,
            },
        .gpio_cfg =
            {
                .mclk = BSP_I2S_MCLK,
                .bclk = BSP_I2S_BCLK,
                .ws = BSP_I2S_WS,
                .dout = BSP_I2S_DOUT,
                .din = BSP_I2S_DIN,
                .invert_flags = {.mclk_inv = false, .bclk_inv = false, .ws_inv = false},
            },
    };
    if ((err = i2s_channel_init_std_mode(s_tx, &standard)) != ESP_OK)
        return err;
    if ((err = i2s_channel_init_std_mode(s_rx, &standard)) != ESP_OK)
        return err;
    i2s_channel_enable(s_tx);
    i2s_channel_enable(s_rx);
    return ESP_OK;
}

esp_err_t bsp_audio_init(void) {
    if (s_input_device && s_output_device)
        return ESP_OK;
    esp_err_t err = bsp_i2c_init();
    if (err != ESP_OK)
        return err;
    if ((err = init_i2s()) != ESP_OK)
        return err;

    audio_codec_i2c_cfg_t control_config = {
        .port = BSP_I2C_PORT,
        .addr = BSP_I2C_ES8311_ADDR << 1,
        .bus_handle = bsp_i2c_bus(),
    };
    const audio_codec_ctrl_if_t *control = audio_codec_new_i2c_ctrl(&control_config);
    audio_codec_i2s_cfg_t data_config = {
        .port = BSP_I2S_PORT, .rx_handle = s_rx, .tx_handle = s_tx};
    const audio_codec_data_if_t *data = audio_codec_new_i2s_data(&data_config);
    if (!control || !data)
        return ESP_FAIL;

    es8311_codec_cfg_t codec_config = {};
    codec_config.ctrl_if = control;
    codec_config.gpio_if = audio_codec_new_gpio();
    codec_config.codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH;
    codec_config.pa_pin = BSP_AUDIO_PA;
    codec_config.pa_reverted = false;
    codec_config.master_mode = false;
    // M301 product codec ES8311 configuration derives its internal clock from BCLK.
    codec_config.use_mclk = false;
    codec_config.hw_gain.pa_voltage = 5.0f;
    codec_config.hw_gain.codec_dac_voltage = 3.3f;
    const audio_codec_if_t *codec = es8311_codec_new(&codec_config);
    if (!codec)
        return ESP_FAIL;

    esp_codec_dev_cfg_t output_config = {
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
        .codec_if = codec,
        .data_if = data,
    };
    s_output_device = esp_codec_dev_new(&output_config);
    if (!s_output_device)
        return ESP_FAIL;

    esp_codec_dev_cfg_t input_config = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = codec,
        .data_if = data,
    };
    s_input_device = esp_codec_dev_new(&input_config);
    if (!s_input_device)
        return ESP_FAIL;
    ESP_LOGI(TAG, "ES8311 ready");
    return ESP_OK;
}

esp_err_t bsp_audio_open(uint32_t sample_rate, uint8_t bits, uint8_t channels) {
    if (!s_input_device || !s_output_device)
        return ESP_ERR_INVALID_STATE;
    if (s_open && s_rate == sample_rate && s_bits == bits && s_channels == channels)
        return ESP_OK;
    if (s_open) {
        esp_codec_dev_close(s_input_device);
        esp_codec_dev_close(s_output_device);
        s_open = false;
    }
    esp_codec_dev_sample_info_t format = {
        .bits_per_sample = bits,
        .channel = channels,
        .channel_mask = ESP_CODEC_DEV_MAKE_CHANNEL_MASK(0),
        .sample_rate = sample_rate,
        .mclk_multiple = 0,
    };
    if (esp_codec_dev_open(s_input_device, &format) != 0)
        return ESP_FAIL;
    if (esp_codec_dev_set_in_gain(s_input_device, 30.0f) != 0) {
        esp_codec_dev_close(s_input_device);
        return ESP_FAIL;
    }
    if (esp_codec_dev_open(s_output_device, &format) != 0) {
        esp_codec_dev_close(s_input_device);
        return ESP_FAIL;
    }
    s_rate = sample_rate;
    s_bits = bits;
    s_channels = channels;
    s_open = true;
    return ESP_OK;
}

esp_err_t bsp_audio_write(const void *pcm, size_t bytes) {
    if (!s_open)
        return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_write(s_output_device, const_cast<void *>(pcm), bytes) == 0 ? ESP_OK
                                                                                     : ESP_FAIL;
}

esp_err_t bsp_audio_read(void *pcm, size_t bytes) {
    if (!s_open)
        return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_read(s_input_device, pcm, bytes) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_audio_set_volume(uint8_t percent) {
    if (!s_output_device)
        return ESP_ERR_INVALID_STATE;
    if (percent > 100)
        return ESP_ERR_INVALID_ARG;
    return esp_codec_dev_set_out_vol(s_output_device, percent) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_audio_set_input_gain(float db) {
    if (!s_input_device)
        return ESP_ERR_INVALID_STATE;
    return esp_codec_dev_set_in_gain(s_input_device, db) == 0 ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_audio_close(void) {
    if (!s_open)
        return ESP_OK;
    const int input_result = esp_codec_dev_close(s_input_device);
    const int output_result = esp_codec_dev_close(s_output_device);
    s_open = false;
    return input_result == 0 && output_result == 0 ? ESP_OK : ESP_FAIL;
}
