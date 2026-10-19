#include "bsp_wakeup.hpp"
#include "bsp_power.hpp"
#include "bsp_pins.h"
#include "button_gpio.h"
#include "iot_button.h"
#include "port_gx8002.hpp"

static GX8002 s_gx8002;
static button_handle_t s_irq_button;
static bsp_wakeup_callback_t s_callback;
static void *s_callback_user_data;
static bool s_ready;

static void on_wakeup(void *, void *) {
    if (s_callback)
        s_callback(s_callback_user_data);
}

esp_err_t bsp_wakeup_init() {
    if (s_ready)
        return ESP_OK;
    if (bsp_power_set_wakeup_rails(false) != ESP_OK)
        return ESP_FAIL;
    s_gx8002.setOpenPowerCallback([] { return bsp_power_set_wakeup_rails(true) == ESP_OK; });
    s_gx8002.setClosePowerCallback([] { return bsp_power_set_wakeup_rails(false) == ESP_OK; });
    if (!s_gx8002.gx8002_init(BSP_I2C_PORT))
        return ESP_FAIL;

    const button_config_t button_config = {
        .long_press_time = 3000,
        .short_press_time = 0,
    };
    const button_gpio_config_t gpio_config = {
        .gpio_num = BSP_GX8002_IRQ,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = false,
    };
    esp_err_t err = iot_button_new_gpio_device(&button_config, &gpio_config, &s_irq_button);
    if (err != ESP_OK)
        return err;
    err = iot_button_register_cb(s_irq_button, BUTTON_PRESS_DOWN, nullptr, on_wakeup, nullptr);
    if (err != ESP_OK)
        return err;

    s_ready = true;
    return ESP_OK;
}

void bsp_wakeup_set_callback(bsp_wakeup_callback_t callback, void *user_data) {
    s_callback = callback;
    s_callback_user_data = user_data;
}

esp_err_t bsp_wakeup_enable(bool enabled) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    const bool ok = enabled ? s_gx8002.gx8002_enable() : s_gx8002.gx8002_disable();
    return ok ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_wakeup_upgrade(const char *boot_path, const char *firmware_path) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (!boot_path || !firmware_path)
        return ESP_ERR_INVALID_ARG;
    return s_gx8002.upgrade(boot_path, firmware_path) ? ESP_OK : ESP_FAIL;
}

std::string bsp_wakeup_version() { return s_ready ? s_gx8002.get_version() : std::string{}; }
