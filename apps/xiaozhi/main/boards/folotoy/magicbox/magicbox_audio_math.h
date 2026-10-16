#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>

namespace magicbox {
constexpr int KnobLevelQ15(int raw) {
    return 1638 + std::clamp(raw, 0, 4095) * (32768 - 1638) / 4095;
}

constexpr int16_t DecodeMicrophone(int32_t raw) {
    return static_cast<int16_t>(
        std::clamp(raw >> 13, static_cast<int32_t>(INT16_MIN), static_cast<int32_t>(INT16_MAX)));
}

constexpr int32_t EncodeSpeaker(int16_t sample, int level) {
    return static_cast<int32_t>(static_cast<int64_t>(sample) * 65536 * std::clamp(level, 0, 32768) /
                                32768);
}
}  // namespace magicbox
