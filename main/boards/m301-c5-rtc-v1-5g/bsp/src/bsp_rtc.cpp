#include "bsp_rtc.hpp"
#include "bsp_i2c.h"
#include "bsp_pins.h"
#include "port_bm8563.hpp"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "freertos/task.h"

static BM8563 s_rtc;
static bool s_ready;
static TaskHandle_t s_task;
static bsp_rtc_callback_t s_callback;
static void *s_callback_data;

static void IRAM_ATTR rtc_irq(void *) {
    BaseType_t wake = pdFALSE;
    vTaskNotifyGiveFromISR(s_task, &wake);
    portYIELD_FROM_ISR(wake);
}

static void rtc_task(void *) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        s_rtc.resetRtcAlarm();
        if (s_callback)
            s_callback(s_callback_data);
    }
}

esp_err_t bsp_rtc_init() {
    if (s_ready)
        return ESP_OK;
    esp_err_t err = bsp_i2c_init();
    if (err != ESP_OK)
        return err;
    err = s_rtc.init(BSP_I2C_BM8563_ADDR, 0, BSP_I2C_PORT);
    if (err != ESP_OK)
        return err;
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << BSP_RTC_IRQ,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    if ((err = gpio_config(&config)) != ESP_OK)
        return err;
    err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
        return err;
    if (xTaskCreate(rtc_task, "rtc_irq", 3072, nullptr, 7, &s_task) != pdPASS)
        return ESP_ERR_NO_MEM;
    if ((err = gpio_isr_handler_add(BSP_RTC_IRQ, rtc_irq, nullptr)) != ESP_OK)
        return err;
    s_ready = true;
    return ESP_OK;
}

esp_err_t bsp_rtc_set_callback(bsp_rtc_callback_t callback, void *user_data) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_callback = callback;
    s_callback_data = user_data;
    return ESP_OK;
}

esp_err_t bsp_rtc_set_time(const timeval *time) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (!time)
        return ESP_ERR_INVALID_ARG;
    timeval copy = *time;
    s_rtc.setRtcDateTime(&copy);
    return ESP_OK;
}

esp_err_t bsp_rtc_set_alarm(uint8_t hour, uint8_t minute, uint8_t day, uint8_t weekday) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_rtc.disableRtcAlarm();
    s_rtc.setRtcAlarm(hour, minute, day, weekday);
    s_rtc.enableRtcAlarm();
    return ESP_OK;
}

esp_err_t bsp_rtc_set_alarm_after(int seconds) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (seconds <= 0)
        return ESP_ERR_INVALID_ARG;
    s_rtc.disableRtcAlarm();
    if (s_rtc.setRtcAlarmBySeconds(seconds) < 0)
        return ESP_FAIL;
    s_rtc.enableRtcAlarm();
    return ESP_OK;
}

esp_err_t bsp_rtc_enable_alarm(bool enabled) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (enabled)
        s_rtc.enableRtcAlarm();
    else
        s_rtc.disableRtcAlarm();
    return ESP_OK;
}

esp_err_t bsp_rtc_clear_alarm() {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_rtc.resetRtcAlarm();
    return ESP_OK;
}

bool bsp_rtc_caused_wakeup() {
    return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1 &&
           (esp_sleep_get_ext1_wakeup_status() & (1ULL << BSP_RTC_IRQ));
}
