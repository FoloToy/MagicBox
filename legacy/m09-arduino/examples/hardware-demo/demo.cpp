// Interactive local hardware diagnostics; no cloud or product behavior.
#include <Arduino.h>
#include <bsp.h>
#include <bsp_pins.h>
#include <stdlib.h>
#include <mbedtls/base64.h>

// Preserve the original 12-bit ADC span; measured stopping positions are not fixed endpoints.
#ifndef MAGICBOX_DEMO_KNOB_MIN_RAW
#define MAGICBOX_DEMO_KNOB_MIN_RAW 0
#endif
#ifndef MAGICBOX_DEMO_KNOB_MAX_RAW
#define MAGICBOX_DEMO_KNOB_MAX_RAW 4095
#endif
static_assert(MAGICBOX_DEMO_KNOB_MIN_RAW >= 0 && MAGICBOX_DEMO_KNOB_MAX_RAW <= 4095 &&
                  MAGICBOX_DEMO_KNOB_MIN_RAW < MAGICBOX_DEMO_KNOB_MAX_RAW,
              "Invalid knob endpoints");

static bool audio_ready;
static int16_t *recorded_pcm;
static size_t recorded_samples;
static size_t playback_samples;
static unsigned playback_gain = 8;
static bool keys[BSP_KEY_COUNT];
static uint32_t last_status;
static uint16_t knob_min = 4095, knob_max = 0;
static float volume_level;
static bool volume_test;
static uint32_t last_volume_tone;

static float knob_to_level(float raw) {
    float fraction = (raw - MAGICBOX_DEMO_KNOB_MIN_RAW) /
                     (MAGICBOX_DEMO_KNOB_MAX_RAW - MAGICBOX_DEMO_KNOB_MIN_RAW);
    fraction = constrain(fraction, 0.0f, 1.0f);
    return (0.05f + fraction * 0.95f) * 4095.0f;
}

static unsigned volume_percent() { return static_cast<unsigned>(volume_level * 100 / 4095 + 0.5f); }

static uint16_t update_volume() {
    unsigned total = 0;
    for (unsigned i = 0; i < 4; ++i)
        total += bsp::knob_raw();
    const float target = knob_to_level(total / 4.0f);
    volume_level += (target - volume_level) * 0.2f;
    if (abs(target - volume_level) < 0.75f)
        volume_level = target;
    return static_cast<uint16_t>(volume_level + 0.5f);
}

static void tone(unsigned duration_ms = 100) {
    if (!audio_ready)
        return;
    const size_t samples = BSP_SAMPLE_RATE * duration_ms / 1000;
    int16_t pcm[256];
    size_t sent = 0;
    esp_err_t err = ESP_OK;
    while (sent < samples) {
        const size_t chunk = min(static_cast<size_t>(256), samples - sent);
        const float before = volume_level;
        const float after = update_volume();
        for (size_t i = 0; i < chunk; ++i) {
            const float level = before + (after - before) * (i + 1) / chunk;
            const int value = (sent + i) % 32 < 16 ? 1000 : -1000;
            pcm[i] = static_cast<int16_t>(value * level / 4095.0f);
        }
        size_t count = 0;
        err = bsp::audio_write(pcm, chunk, &count, 1000);
        if (err != ESP_OK || !count)
            break;
        sent += count;
    }
    Serial.printf("TONE result=%s samples=%u volume_pct=%u\n", esp_err_to_name(err), (unsigned)sent,
                  volume_percent());
}

static void colors() {
    const uint8_t rgb[][3] = {{12, 0, 0}, {0, 12, 0}, {0, 0, 12}, {0, 0, 0}};
    const char *names[] = {"RED", "GREEN", "BLUE", "OFF"};
    for (unsigned i = 0; i < 4; ++i) {
        Serial.printf("LED %s\n", names[i]);
        bsp::rgb(rgb[i][0], rgb[i][1], rgb[i][2]);
        delay(800);
    }
    bsp::rgb(0, 8, 0);
}

static void playback() {
    if (!audio_ready || !recorded_samples) {
        Serial.println("PLAYBACK no recording");
        return;
    }
    bsp::rgb(0, 0, 12);
    size_t sent = 0, limited = 0;
    int16_t amplified[256];
    esp_err_t err = ESP_OK;
    const uint32_t started = millis();
    while (sent < playback_samples) {
        size_t count = 0;
        const size_t chunk = min(static_cast<size_t>(256), playback_samples - sent);
        const float before = volume_level;
        const float after = update_volume();
        for (size_t i = 0; i < chunk; ++i) {
            const float level = before + (after - before) * (i + 1) / chunk;
            int32_t value =
                static_cast<int32_t>(static_cast<int32_t>(recorded_pcm[sent + i]) *
                                     static_cast<int32_t>(playback_gain) * level / 4095.0f);
            if (value > 30000) {
                value = 30000;
                ++limited;
            } else if (value < -30000) {
                value = -30000;
                ++limited;
            }
            amplified[i] = static_cast<int16_t>(value);
        }
        err = bsp::audio_write(amplified, chunk, &count, 200);
        if (err != ESP_OK || !count)
            break;
        sent += count;
    }
    // The I2S driver queues DMA data; allow its final samples to finish before changing the LED.
    delay(80);
    Serial.printf("PLAYBACK result=%s samples=%u gain=%u limited=%u elapsed_ms=%u\n",
                  esp_err_to_name(err), (unsigned)sent, playback_gain, (unsigned)limited,
                  (unsigned)(millis() - started));
    bsp::rgb(0, 8, 0);
}

static void dump_recording() {
    if (!recorded_samples) {
        Serial.println("DUMP no recording");
        return;
    }
    Serial.printf("DUMP BEGIN rate=%u samples=%u\n", BSP_SAMPLE_RATE, (unsigned)recorded_samples);
    const auto *bytes = reinterpret_cast<const unsigned char *>(recorded_pcm);
    const size_t total = recorded_samples * sizeof(int16_t);
    for (size_t offset = 0; offset < total; offset += 192) {
        unsigned char encoded[257];
        size_t length = 0;
        if (mbedtls_base64_encode(encoded, sizeof(encoded), &length, bytes + offset,
                                  min(static_cast<size_t>(192), total - offset)) != 0) {
            Serial.println("DUMP ERROR encode");
            return;
        }
        Serial.print("PCM ");
        Serial.write(encoded, length);
        Serial.println();
    }
    Serial.println("DUMP END");
}

static void record_playback(bool held = false) {
    if (!audio_ready)
        return;
    const size_t capacity = BSP_SAMPLE_RATE * 10;
    const size_t samples = BSP_SAMPLE_RATE * (held ? 10 : 5);
    if (!recorded_pcm)
        recorded_pcm = static_cast<int16_t *>(malloc(capacity * sizeof(int16_t)));
    int16_t *pcm = recorded_pcm;
    recorded_samples = playback_samples = 0;
    if (!pcm) {
        Serial.println("RECORD no memory");
        return;
    }
    Serial.printf("RECORD start mode=%s max_seconds=%u\n", held ? "hold-Voice" : "fixed",
                  held ? 10 : 5);
    bsp::rgb(12, 0, 0);
    // Discard queued idle audio and the initial mechanical press transient.
    int16_t discarded[256];
    for (unsigned i = 0; i < 10; ++i) {
        size_t count = 0;
        if (bsp::audio_read(discarded, 256, &count, 100) != ESP_OK || !count)
            break;
    }
    const uint32_t started = millis();
    uint32_t released_at = 0;
    size_t used = 0;
    int peak = 0;
    esp_err_t err = ESP_OK;
    while (used < samples) {
        if (held) {
            if (bsp::button_pressed(1))
                released_at = 0;
            else if (!released_at)
                released_at = millis();
            else if (millis() - released_at >= 30)
                break;
        }
        size_t count = 0;
        const size_t chunk = min(static_cast<size_t>(256), samples - used);
        err = bsp::audio_read(pcm + used, chunk, &count, 200);
        if (err != ESP_OK || !count)
            break;
        for (size_t i = 0; i < count; ++i) {
            int value = pcm[used + i];
            if (value < 0)
                value = -value;
            if (value > peak)
                peak = value;
        }
        used += count;
    }
    recorded_samples = used;
    playback_samples = used;
    // Keep raw PCM for export; omit the mechanical release transient from audition.
    if (held && released_at && used > BSP_SAMPLE_RATE / 12)
        playback_samples -= BSP_SAMPLE_RATE * 80 / 1000;
    Serial.printf("RECORD result=%s samples=%u peak=%d elapsed_ms=%u\n", esp_err_to_name(err),
                  (unsigned)used, peak, (unsigned)(millis() - started));
    playback();
}

void setup() {
    Serial.begin(115200);
    bsp::begin();
    volume_level = knob_to_level(bsp::knob_raw());
    Serial.printf("BOARD %s flash=%u psram=%u heap=%u\n", bsp::board_name(), ESP.getFlashChipSize(),
                  ESP.getPsramSize(), ESP.getFreeHeap());
    auto err = bsp::audio_begin();
    audio_ready = err == ESP_OK;
    Serial.printf("AUDIO init=%s\n", esp_err_to_name(err));
    Serial.println("COMMANDS t=tone l=RGB-cycle r=5s-record/playback p=replay d=export-PCM "
                   "g=cycle-gain v=toggle-volume-test h=help");
    tone();
    bsp::rgb(0, 8, 0);
}

void loop() {
    while (Serial.available()) {
        const char command = Serial.read();
        if (command == 't')
            tone();
        else if (command == 'l')
            colors();
        else if (command == 'r')
            record_playback();
        else if (command == 'p')
            playback();
        else if (command == 'd')
            dump_recording();
        else if (command == 'v') {
            volume_test = !volume_test;
            Serial.printf("VOLUME test=%u\n", volume_test);
        } else if (command == 'g') {
            playback_gain = playback_gain >= 8 ? 1 : playback_gain * 2;
            Serial.printf("GAIN value=%u\n", playback_gain);
        } else if (command == 'h')
            Serial.println("COMMANDS t=tone l=RGB-cycle r=5s-record/playback p=replay d=export-PCM "
                           "g=cycle-gain v=toggle-volume-test h=help");
    }
    for (unsigned i = 0; i < BSP_KEY_COUNT; ++i) {
        bool pressed = bsp::button_pressed(i);
        if (pressed != keys[i]) {
            keys[i] = pressed;
            Serial.printf("BUTTON index=%u gpio=%d pressed=%u\n", i, BSP_KEYS[i], pressed);
            if (pressed && i == 1)
                record_playback(true);
            else if (pressed && i == 0) {
                tone();
                colors();
            }
        }
    }
    update_volume();
    if (volume_test && millis() - last_volume_tone >= 1000) {
        last_volume_tone = millis();
        tone(240);
    }
    const uint16_t knob = bsp::knob_raw();
    knob_min = min(knob_min, knob);
    knob_max = max(knob_max, knob);
    if (millis() - last_status >= 1000) {
        last_status = millis();
        int16_t pcm[256];
        size_t count = 0;
        int peak = 0;
        auto err = bsp::audio_read(pcm, 256, &count, 100);
        for (size_t i = 0; i < count; ++i) {
            int value = pcm[i];
            if (value < 0)
                value = -value;
            if (value > peak)
                peak = value;
        }
        Serial.printf("STATUS knob=%u mic=%s samples=%u peak=%d heap=%u\n", knob,
                      esp_err_to_name(err), (unsigned)count, peak, ESP.getFreeHeap());
        Serial.printf("KNOB min=%u max=%u\n", knob_min, knob_max);
        Serial.printf("VOLUME raw=%u level=%u percent=%u test=%u\n", knob, (unsigned)volume_level,
                      volume_percent(), volume_test);
        knob_min = 4095;
        knob_max = 0;
    }
    delay(10);
}
