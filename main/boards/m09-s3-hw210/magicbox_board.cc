#include <esp_pm.h>
#include <esp_wifi.h>
#include <sys/time.h>
#include "application.h"
#include "bsp_motion.h"
#include "button.h"
#include "config.h"
#include "led/single_led.h"
#include "magicbox_audio_codec.h"
#include "wifi_board.h"

#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>
#include <ssid_manager.h>

namespace {
constexpr char kTag[] = "FoloToyMagicBox";
constexpr adc_channel_t kKnobChannel = ADC_CHANNEL_7;  // GPIO8, ADC1 on ESP32-S3
}  // namespace

class MagicBoxBoard final : public WifiBoard {
public:
    MagicBoxBoard()
        : mode_button_(kModeButton, false, 3000, 0, true),
          voice_button_(kVoiceButton, false, 0, 0, true),
          external_mode_button_(kExternalModeButton, false, 3000, 0, true) {
        InitializeKnob();
        InitializeButtons();
        if (!motion_.Begin())
            ESP_LOGE(kTag, "Motion setup failed; automatic sleep disabled");
        sleep_callbacks_.exit_cb = [](int64_t us, void* arg) -> esp_err_t {
            auto* self = static_cast<MagicBoxBoard*>(arg);
            self->sleep_count_.fetch_add(1, std::memory_order_relaxed);
            self->sleep_us_.fetch_add(static_cast<uint32_t>(us), std::memory_order_relaxed);
            return ESP_OK;
        };
        sleep_callbacks_.exit_cb_user_arg = this;
        ESP_ERROR_CHECK(esp_pm_light_sleep_register_cbs(&sleep_callbacks_));
        ESP_LOGI(kTag, "M09 / HWv2.1.0, 16MB flash / 8MB OPI PSRAM; Mode=4 Voice=3 RGB=46 knob=8");
        if (xTaskCreate(KnobTask, "magicbox_knob", 3072, this, 2, &knob_task_) != pdPASS) {
            ESP_LOGE(kTag, "Cannot start knob sampling task");
            ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
        }
    }

    ~MagicBoxBoard() override {
        esp_pm_light_sleep_unregister_cbs(&sleep_callbacks_);
        if (knob_task_)
            vTaskDelete(knob_task_);
        if (adc_)
            ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_));
    }

    void StartNetwork() override {
#ifdef CONFIG_FOLOTOY_MAGICBOX_DEMO_WIFI
        auto& ssids = SsidManager::GetInstance();
        // Seed only an unconfigured device; keep networks saved through provisioning.
        if (ssids.GetSsidList().empty()) {
            ssids.AddSsid("FOLOTOY_LAND", "makeitanything");
            ESP_LOGI(kTag, "Installed default WiFi network");
        }
#endif
        WifiBoard::StartNetwork();
    }

    AudioCodec* GetAudioCodec() override { return &codec_; }

    Led* GetLed() override {
        static SingleLed led(kStatusLed);
        return &led;
    }

private:
    MagicBoxAudioCodec codec_;
    Button mode_button_;
    Button voice_button_;
    // External GPIO6 Mode input verified on this physical HW2.1.0 unit.
    Button external_mode_button_;
    adc_oneshot_unit_handle_t adc_ = nullptr;
    TaskHandle_t knob_task_ = nullptr;
    MagicBoxMotion motion_;
    esp_pm_sleep_cbs_register_config_t sleep_callbacks_ = {};
    std::atomic<uint32_t> sleep_count_{0}, sleep_us_{0};
    bool sleeping_ = false;  // Main application task owns sleep transitions.
    int64_t last_activity_ = esp_timer_get_time();
    int64_t sleep_started_ = 0;
    struct timeval sleep_wall_ = {};

    void Wake(const char* reason) {
        last_activity_ = esp_timer_get_time();
        if (!sleeping_)
            return;
        esp_pm_config_t pm = {240, 240, false};
        ESP_ERROR_CHECK(esp_pm_configure(&pm));
        sleeping_ = false;
        WifiBoard::SetPowerSaveLevel(PowerSaveLevel::PERFORMANCE);
        wifi_ap_record_t ap = {};
        bool connected = esp_wifi_sta_get_ap_info(&ap) == ESP_OK;
        struct timeval now;
        gettimeofday(&now, nullptr);
        const int64_t wall_ms =
            (now.tv_sec - sleep_wall_.tv_sec) * 1000LL + (now.tv_usec - sleep_wall_.tv_usec) / 1000;
        ESP_LOGI(
            kTag,
            "WAKE reason=%s WiFi=%s elapsed_ms=%lld clock_ms=%lld light_sleeps=%lu slept_ms=%lu",
            reason, connected ? "associated" : "reconnecting",
            (esp_timer_get_time() - sleep_started_) / 1000, wall_ms,
            static_cast<unsigned long>(sleep_count_.load()),
            static_cast<unsigned long>(sleep_us_.load() / 1000));
    }

    void PowerTick() {
        if (!motion_.Ready())
            return;
        auto& app = Application::GetInstance();
        if (!app.CanEnterSleepMode()) {
            Wake("application active");
            return;
        }
        if (sleeping_ || esp_timer_get_time() - last_activity_ < 60000000)
            return;
        wifi_ap_record_t ap = {};
        if (esp_wifi_sta_get_ap_info(&ap) != ESP_OK)
            return;
        GetAudioCodec()->EnableInput(false);
        GetAudioCodec()->EnableOutput(false);
        WifiBoard::SetPowerSaveLevel(PowerSaveLevel::BALANCED);
        sleep_count_.store(0);
        sleep_us_.store(0);
        sleep_started_ = esp_timer_get_time();
        gettimeofday(&sleep_wall_, nullptr);
        esp_pm_config_t pm = {240, 40, true};
        auto err = esp_pm_configure(&pm);
        if (err != ESP_OK) {
            ESP_LOGE(kTag, "Light sleep configuration failed: %s", esp_err_to_name(err));
            last_activity_ = esp_timer_get_time();
            return;
        }
        sleeping_ = true;
        ESP_LOGI(kTag,
                 "SLEEP automatic light sleep after 60s idle; WiFi retained, motion/buttons wake");
    }

    void InitializeKnob() {
        adc_oneshot_unit_init_cfg_t unit = {};
        unit.unit_id = ADC_UNIT_1;
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit, &adc_));
        adc_oneshot_chan_cfg_t channel = {};
        channel.atten = ADC_ATTEN_DB_12;
        channel.bitwidth = ADC_BITWIDTH_12;
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_, kKnobChannel, &channel));
        // Seed before audio starts, so startup prompts respect the physical position.
        int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_, kKnobChannel, &raw));
        for (int i = 0; i < 64; ++i)
            codec_.SetKnobRaw(raw);
    }

    static void KnobTask(void* context) {
        auto* board = static_cast<MagicBoxBoard*>(context);
        int previous = -100;
        unsigned errors = 0;
        unsigned ticks = 0;
        while (true) {
            if (board->motion_.Poll()) {
                Application::GetInstance().Schedule([board]() { board->Wake("motion GPIO14"); });
            }
            if (++ticks % 10 == 0) {
                Application::GetInstance().Schedule([board]() { board->PowerTick(); });
            }
            int total = 0;
            bool valid = true;
            for (int i = 0; i < 4; ++i) {
                int value = 0;
                if (adc_oneshot_read(board->adc_, kKnobChannel, &value) != ESP_OK) {
                    valid = false;
                    break;
                }
                total += value;
            }
            if (valid) {
                board->codec_.SetKnobRaw(total / 4);
                int volume = board->codec_.KnobPercent();
                if (std::abs(volume - previous) >= 5) {
                    ESP_LOGI(kTag, "Knob raw=%d volume=%d%%", total / 4, volume);
                    previous = volume;
                }
            } else if (++errors % 100 == 1) {
                ESP_LOGW(kTag, "ADC read failed; keeping the previous volume");
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    void InitializeButtons() {
        voice_button_.OnPressDown([this]() {
            Application::GetInstance().Schedule([this]() {
                Wake("Voice");
                Application::GetInstance().StartListening();
            });
        });
        voice_button_.OnPressUp([]() {
            Application::GetInstance().Schedule(
                []() { Application::GetInstance().StopListening(); });
        });
        ConfigureModeButton(mode_button_, "Mode GPIO4");
        ConfigureModeButton(external_mode_button_, "External Mode GPIO6");
    }

    void ConfigureModeButton(Button& button, const char* name) {
        button.OnPressDown([this, name]() {
            ESP_LOGI(kTag, "%s pressed", name);
            Application::GetInstance().Schedule([this]() { Wake("Mode"); });
        });
        button.OnPressUp([name]() { ESP_LOGI(kTag, "%s released", name); });
        button.OnClick([this]() {
            Application::GetInstance().Schedule([this]() {
                if (Application::GetInstance().GetDeviceState() == kDeviceStateStarting ||
                    IsInWifiConfigMode()) {
                    EnterWifiConfigMode();
                } else {
                    Application::GetInstance().ToggleChatState();
                }
            });
        });
        button.OnLongPress(
            [this]() { Application::GetInstance().Schedule([this]() { EnterWifiConfigMode(); }); });
    }
};

DECLARE_BOARD(MagicBoxBoard);
