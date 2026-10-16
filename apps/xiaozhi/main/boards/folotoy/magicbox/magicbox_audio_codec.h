#pragma once

#include "codecs/no_audio_codec.h"
#include "config.h"
#include "magicbox_audio_math.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <limits>

// Board-local PCM gain and physical volume. The timer never writes volume to NVS.
class MagicBoxAudioCodec final : public NoAudioCodecSimplex {
public:
    MagicBoxAudioCodec()
        : NoAudioCodecSimplex(
              kInputSampleRate, kOutputSampleRate, static_cast<gpio_num_t>(BSP_SPEAKER_BCLK),
              static_cast<gpio_num_t>(BSP_SPEAKER_WS), static_cast<gpio_num_t>(BSP_SPEAKER_DATA),
              I2S_STD_SLOT_LEFT, static_cast<gpio_num_t>(BSP_MIC_BCLK),
              static_cast<gpio_num_t>(BSP_MIC_WS), static_cast<gpio_num_t>(BSP_MIC_DATA),
              I2S_STD_SLOT_LEFT) {
        output_volume_ = 100;
    }

    void SetKnobRaw(int raw) {
        const int desired = magicbox::KnobLevelQ15(raw);
        int current = knob_level_.load(std::memory_order_relaxed);
        current += (desired - current) / 5;
        if (std::abs(desired - current) < 5)
            current = desired;
        knob_level_.store(current, std::memory_order_relaxed);
    }

    int KnobPercent() const {
        return (knob_level_.load(std::memory_order_relaxed) * 100 + 16384) / 32768;
    }

    void SetOutputVolume(int volume) override {
        std::lock_guard<std::mutex> lock(data_if_mutex_);
        AudioCodec::SetOutputVolume(std::clamp(volume, 0, 100));
    }

protected:
    int Read(int16_t* destination, int samples) override {
        std::array<int32_t, 64> raw{};
        int received = 0;
        while (received < samples) {
            const int count = std::min(samples - received, static_cast<int>(raw.size()));
            size_t bytes = 0;
            auto err =
                i2s_channel_read(rx_handle_, raw.data(), count * sizeof(int32_t), &bytes, 200);
            if (err != ESP_OK || bytes == 0)
                break;
            const int frames = bytes / sizeof(int32_t);
            for (int i = 0; i < frames; ++i) {
                // The hardware diagnostic's accepted 8x gain: 32-bit I2S -> 16-bit PCM.
                destination[received + i] = magicbox::DecodeMicrophone(raw[i]);
            }
            received += frames;
        }
        std::fill(destination + received, destination + samples, 0);
        return received;
    }

    int Write(const int16_t* data, int samples) override {
        std::lock_guard<std::mutex> lock(data_if_mutex_);
        std::array<int32_t, 64> output{};
        const int master = std::clamp(output_volume_, 0, 100);
        int sent = 0;
        while (sent < samples) {
            const int count = std::min(samples - sent, static_cast<int>(output.size()));
            const int target =
                knob_level_.load(std::memory_order_relaxed) * master * master / 10000;
            for (int i = 0; i < count; ++i) {
                const int level = previous_level_ + (target - previous_level_) * (i + 1) / count;
                output[i] = magicbox::EncodeSpeaker(data[sent + i], level);
            }
            previous_level_ = target;
            size_t bytes = 0;
            auto err =
                i2s_channel_write(tx_handle_, output.data(), count * sizeof(int32_t), &bytes, 200);
            if (err != ESP_OK || bytes == 0)
                break;
            sent += bytes / sizeof(int32_t);
        }
        return sent;
    }

private:
    std::atomic<int> knob_level_{1638};
    int previous_level_ = 1638;
};
