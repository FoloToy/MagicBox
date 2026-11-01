#pragma once

#include "esp_err.h"
#include <cstdint>

class AXP2101;

enum bsp_power_event_t : uint32_t {
    BSP_POWER_EVENT_LOW_BATTERY_CRITICAL = 1U << 0,
    BSP_POWER_EVENT_LOW_BATTERY_WARNING = 1U << 1,
    BSP_POWER_EVENT_VBUS_INSERTED = 1U << 2,
    BSP_POWER_EVENT_VBUS_REMOVED = 1U << 3,
    BSP_POWER_EVENT_BATTERY_INSERTED = 1U << 4,
    BSP_POWER_EVENT_BATTERY_REMOVED = 1U << 5,
    BSP_POWER_EVENT_CHARGE_STARTED = 1U << 6,
    BSP_POWER_EVENT_CHARGE_DONE = 1U << 7,
    BSP_POWER_EVENT_TEMPERATURE_FAULT = 1U << 8,
    BSP_POWER_EVENT_OVERCURRENT = 1U << 9,
    BSP_POWER_EVENT_CHARGE_TIMEOUT = 1U << 10,
    BSP_POWER_EVENT_POWER_KEY = 1U << 11,
};

struct bsp_power_status_t {
    int battery_percent;
    uint16_t battery_mv;
    uint16_t system_mv;
    uint16_t vbus_mv;
    bool battery_present;
    bool vbus_present;
    bool charging;
    int power_on_source;
    int power_off_source;
};

using bsp_power_callback_t = void (*)(uint32_t events, void *user_data);

esp_err_t bsp_power_init();
int bsp_power_battery_percent();
bool bsp_power_battery_present();
bool bsp_power_is_charging();
esp_err_t bsp_power_get_status(bsp_power_status_t *status);
esp_err_t bsp_power_set_callback(bsp_power_callback_t callback, void *user_data);
esp_err_t bsp_power_set_charging_enabled(bool enabled);
esp_err_t bsp_power_set_wakeup_rails(bool enabled);
// These calls intentionally do not return on working hardware.
esp_err_t bsp_power_enter_sleep();
esp_err_t bsp_power_shutdown();
// Escape hatch for board-specific AXP2101 rail/register operations.
AXP2101 *bsp_power_driver();
