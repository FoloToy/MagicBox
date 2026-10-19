#include "application.h"
#include "bsp_button.h"
#include "bsp_i2c.h"
#include "bsp_knob.h"
#include "bsp_led.h"
#include "bsp_pins.h"
#include "bsp_power.hpp"
#include "magicbox_c5_audio_codec.h"
#include "wifi_board.h"

#include <esp_log.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace {
constexpr char kTag[] = "MagicBoxM301";

class C5StatusLed final : public Led {
public:
    void OnStateChanged() override {
        switch (Application::GetInstance().GetDeviceState()) {
            case kDeviceStateListening:
                bsp_led_set_rgb(16, 0, 0);
                break;
            case kDeviceStateSpeaking:
            case kDeviceStateAudioTesting:
                bsp_led_set_rgb(0, 0, 16);
                break;
            case kDeviceStateIdle:
                bsp_led_set_rgb(0, 4, 0);
                break;
            case kDeviceStateWifiConfiguring:
            case kDeviceStateConnecting:
                bsp_led_set_rgb(8, 8, 0);
                break;
            case kDeviceStateFatalError:
                bsp_led_set_rgb(24, 0, 0);
                break;
            default:
                bsp_led_set_rgb(4, 0, 4);
                break;
        }
    }
};
}  // namespace

class MagicBoxM301Board final : public WifiBoard {
public:
    MagicBoxM301Board() {
        ESP_ERROR_CHECK(bsp_i2c_init());
        // PMU initialization precedes codec/peripheral initialization.
        ESP_ERROR_CHECK(bsp_power_init());
        ESP_ERROR_CHECK(bsp_led_init());
        ESP_ERROR_CHECK(bsp_knob_init());
        int raw = 0;
        ESP_ERROR_CHECK(bsp_knob_read_raw(&raw));
        codec_ = std::make_unique<MagicBoxC5AudioCodec>(KnobPercent(raw));
        ESP_ERROR_CHECK(bsp_button_init(OnButton, this));
        if (xTaskCreate(KnobTask, "m301_knob", 3072, this, 2, &knob_task_) != pdPASS)
            ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
        ESP_LOGI(kTag, "M301 C5 RTC-v1-5G source; ES8311/AXP2101, Mode=3 Voice=2");
        // RTC/GX8002 remain opt-in BSP APIs. No motion driver, rail sleep or
        // wake-word firmware is assumed from an interrupt pin definition.
    }

    ~MagicBoxM301Board() override {
        if (knob_task_)
            vTaskDelete(knob_task_);
    }

    AudioCodec* GetAudioCodec() override { return codec_.get(); }
    Led* GetLed() override { return &led_; }

    bool GetBatteryLevel(int& level, bool& charging, bool& discharging) override {
        bsp_power_status_t status{};
        if (bsp_power_get_status(&status) != ESP_OK || !status.battery_present)
            return false;
        level = status.battery_percent;
        charging = status.charging;
        discharging = !status.vbus_present;
        return true;
    }

private:
    std::unique_ptr<MagicBoxC5AudioCodec> codec_;
    C5StatusLed led_;
    TaskHandle_t knob_task_ = nullptr;

    static int KnobPercent(int raw) {
        return std::clamp((raw * 100 + BSP_KNOB_RAW_MAX / 2) / BSP_KNOB_RAW_MAX, 0, 100);
    }

    static void OnButton(bsp_button_t button, bsp_button_event_t event, void* context) {
        // BSP callbacks only queue application work; they never block on audio/network.
        auto* board = static_cast<MagicBoxM301Board*>(context);
        Application::GetInstance().Schedule([board, button, event]() {
            auto& app = Application::GetInstance();
            if (button == BSP_BUTTON_VOICE) {
                if (event == BSP_BUTTON_PRESSED)
                    app.StartListening();
                else if (event == BSP_BUTTON_RELEASED)
                    app.StopListening();
            } else if (button == BSP_BUTTON_WIFI) {
                if (event == BSP_BUTTON_LONG_PRESSED) {
                    board->EnterWifiConfigMode();
                } else if (event == BSP_BUTTON_CLICKED) {
                    if (app.GetDeviceState() == kDeviceStateStarting || board->IsInWifiConfigMode())
                        board->EnterWifiConfigMode();
                    else
                        app.ToggleChatState();
                }
            }
        });
    }

    static void KnobTask(void* context) {
        auto* board = static_cast<MagicBoxM301Board*>(context);
        int filtered = -1;
        int previous = -100;
        for (;;) {
            int raw = 0;
            if (bsp_knob_read_raw(&raw) == ESP_OK) {
                filtered = filtered < 0 ? raw : (filtered * 3 + raw) / 4;
                int percent = KnobPercent(filtered);
                if (std::abs(percent - previous) >= 2 || (percent == 0 && previous != 0) ||
                    (percent == 100 && previous != 100)) {
                    previous = percent;
                    Application::GetInstance().Schedule(
                        [board, percent]() { board->codec_->SetOutputVolume(percent); });
                }
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
};

DECLARE_BOARD(MagicBoxM301Board);
