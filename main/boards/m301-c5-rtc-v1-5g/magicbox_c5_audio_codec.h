#pragma once

#include "audio_codec.h"
#include "bsp_audio.h"
#include "bsp_pins.h"

#include <esp_log.h>
#include <algorithm>

// The BSP is the sole I2S/ES8311 owner. Do not create upstream codec channels too.
class MagicBoxC5AudioCodec final : public AudioCodec {
public:
    explicit MagicBoxC5AudioCodec(int volume) {
        duplex_ = true;
        input_sample_rate_ = BSP_AUDIO_RATE;
        output_sample_rate_ = BSP_AUDIO_RATE;
        input_channels_ = output_channels_ = 1;
        input_gain_ = 30.0f;
        output_volume_ = std::clamp(volume, 0, 100);
        ESP_ERROR_CHECK(bsp_audio_init());
        ESP_ERROR_CHECK(bsp_audio_open(BSP_AUDIO_RATE, 16, 1));
        ESP_ERROR_CHECK(bsp_audio_set_input_gain(input_gain_));
        ESP_ERROR_CHECK(bsp_audio_set_volume(0));
    }

    void Start() override {
        // Physical knob position wins over persisted volume, including at boot.
        ESP_LOGI("MagicBoxC5Audio", "ES8311 24kHz mono; physical volume=%d%%", output_volume_);
    }

    void SetOutputVolume(int volume) override {
        // Knob changes must not generate continuous NVS writes.
        output_volume_ = std::clamp(volume, 0, 100);
        ESP_ERROR_CHECK(bsp_audio_set_volume(output_enabled_ ? output_volume_ : 0));
    }

    void SetInputGain(float gain) override {
        ESP_ERROR_CHECK(bsp_audio_set_input_gain(gain));
        input_gain_ = gain;
    }

    void EnableOutput(bool enable) override {
        AudioCodec::EnableOutput(enable);
        ESP_ERROR_CHECK(bsp_audio_set_volume(enable ? output_volume_ : 0));
    }

protected:
    int Read(int16_t* destination, int samples) override {
        return bsp_audio_read(destination, samples * sizeof(int16_t)) == ESP_OK ? samples : 0;
    }

    int Write(const int16_t* data, int samples) override {
        return bsp_audio_write(data, samples * sizeof(int16_t)) == ESP_OK ? samples : 0;
    }
};
