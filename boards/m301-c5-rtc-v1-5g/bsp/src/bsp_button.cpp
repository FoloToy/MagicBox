#include "bsp_button.h"
#include "bsp_pins.h"

#include "button_gpio.h"
#include "iot_button.h"
#include "driver/gpio.h"

#include <cstdint>

static const gpio_num_t BUTTON_PINS[BSP_BUTTON_COUNT] = {
    BSP_BUTTON_WIFI_GPIO,
    BSP_BUTTON_VOICE_GPIO,

};
static button_handle_t s_handles[BSP_BUTTON_COUNT];
static bool s_ready;
static bsp_button_callback_t s_callback;
static void *s_user_data;

static void dispatch(void *user_data, bsp_button_event_t event) {
    if (s_callback) {
        s_callback(static_cast<bsp_button_t>(reinterpret_cast<intptr_t>(user_data)), event,
                   s_user_data);
    }
}
static void on_press(void *, void *user_data) { dispatch(user_data, BSP_BUTTON_PRESSED); }
static void on_release(void *, void *user_data) { dispatch(user_data, BSP_BUTTON_RELEASED); }
static void on_click(void *, void *user_data) { dispatch(user_data, BSP_BUTTON_CLICKED); }
static void on_long_press(void *, void *user_data) { dispatch(user_data, BSP_BUTTON_LONG_PRESSED); }

esp_err_t bsp_button_init(bsp_button_callback_t callback, void *user_data) {
    s_callback = callback;
    s_user_data = user_data;
    if (s_ready)
        return ESP_OK;
    for (int i = 0; i < BSP_BUTTON_COUNT; ++i) {
        const button_config_t button_config = {.long_press_time = 1000, .short_press_time = 50};
        const button_gpio_config_t gpio_config = {
            .gpio_num = BUTTON_PINS[i],
            .active_level = 0,
            .enable_power_save = false,
            .disable_pull = false,
        };
        esp_err_t err = iot_button_new_gpio_device(&button_config, &gpio_config, &s_handles[i]);
        if (err != ESP_OK) {
            for (int j = 0; j < i; ++j) {
                iot_button_delete(s_handles[j]);
                s_handles[j] = nullptr;
            }
            return err;
        }
        void *index = reinterpret_cast<void *>(static_cast<intptr_t>(i));
        err = iot_button_register_cb(s_handles[i], BUTTON_PRESS_DOWN, nullptr, on_press, index);
        if (err == ESP_OK)
            err = iot_button_register_cb(s_handles[i], BUTTON_PRESS_UP, nullptr, on_release, index);
        if (err == ESP_OK)
            err =
                iot_button_register_cb(s_handles[i], BUTTON_SINGLE_CLICK, nullptr, on_click, index);
        if (err == ESP_OK)
            err = iot_button_register_cb(s_handles[i], BUTTON_LONG_PRESS_START, nullptr,
                                         on_long_press, index);
        if (err != ESP_OK) {
            for (int j = 0; j <= i; ++j) {
                iot_button_delete(s_handles[j]);
                s_handles[j] = nullptr;
            }
            return err;
        }
    }
    s_ready = true;
    return ESP_OK;
}

bool bsp_button_is_pressed(bsp_button_t button) {
    return static_cast<unsigned>(button) < BSP_BUTTON_COUNT &&
           gpio_get_level(BUTTON_PINS[button]) == 0;
}
