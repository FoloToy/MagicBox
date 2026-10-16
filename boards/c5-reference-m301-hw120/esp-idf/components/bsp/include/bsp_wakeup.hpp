#pragma once

#include "esp_err.h"
#include <string>

// Callback runs in the button component task and must not block.
using bsp_wakeup_callback_t = void (*)(void *user_data);

// Initialize after bsp_power_init(); the GX8002 rail sequence is timing-sensitive.
esp_err_t bsp_wakeup_init();
void bsp_wakeup_set_callback(bsp_wakeup_callback_t callback, void *user_data);
esp_err_t bsp_wakeup_enable(bool enabled);
esp_err_t bsp_wakeup_upgrade(const char *boot_path, const char *firmware_path);
std::string bsp_wakeup_version();
