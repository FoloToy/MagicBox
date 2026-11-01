#pragma once

#include "esp_err.h"
#include <cstdint>
#include <sys/time.h>

using bsp_rtc_callback_t = void (*)(void *user_data);

esp_err_t bsp_rtc_init();
esp_err_t bsp_rtc_set_callback(bsp_rtc_callback_t callback, void *user_data);
esp_err_t bsp_rtc_set_time(const timeval *time);
esp_err_t bsp_rtc_set_alarm(uint8_t hour, uint8_t minute, uint8_t day, uint8_t weekday);
esp_err_t bsp_rtc_set_alarm_after(int seconds);
esp_err_t bsp_rtc_enable_alarm(bool enabled);
esp_err_t bsp_rtc_clear_alarm();
bool bsp_rtc_caused_wakeup();
