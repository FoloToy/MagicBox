#include "bsp_power.hpp"
#include "bsp_i2c.h"
#include "bsp_pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "port_axp2101.hpp"

static AXP2101 s_pmu;
static bool s_ready;
static TaskHandle_t s_irq_task;
static bsp_power_callback_t s_callback;
static void *s_callback_data;

static void IRAM_ATTR power_irq(void *) {
    BaseType_t wake = pdFALSE;
    vTaskNotifyGiveFromISR(s_irq_task, &wake);
    portYIELD_FROM_ISR(wake);
}

static void power_irq_task(void *) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (!s_ready)
            continue;
        pmu.getIrqStatus();
        uint32_t events = 0;
        if (pmu.isDropWarningLevel1Irq())
            events |= BSP_POWER_EVENT_LOW_BATTERY_CRITICAL;
        if (pmu.isDropWarningLevel2Irq())
            events |= BSP_POWER_EVENT_LOW_BATTERY_WARNING;
        if (pmu.isVbusInsertIrq())
            events |= BSP_POWER_EVENT_VBUS_INSERTED;
        if (pmu.isVbusRemoveIrq())
            events |= BSP_POWER_EVENT_VBUS_REMOVED;
        if (pmu.isBatInsertIrq())
            events |= BSP_POWER_EVENT_BATTERY_INSERTED;
        if (pmu.isBatRemoveIrq())
            events |= BSP_POWER_EVENT_BATTERY_REMOVED;
        if (pmu.isBatChargeStartIrq())
            events |= BSP_POWER_EVENT_CHARGE_STARTED;
        if (pmu.isBatChargeDoneIrq())
            events |= BSP_POWER_EVENT_CHARGE_DONE;
        if (pmu.isBatChargerOverTemperatureIrq() || pmu.isBatChargerUnderTemperatureIrq() ||
            pmu.isBatWorkOverTemperatureIrq() || pmu.isBatWorkUnderTemperatureIrq() ||
            pmu.isBatDieOverTemperatureIrq())
            events |= BSP_POWER_EVENT_TEMPERATURE_FAULT;
        if (pmu.isLdoOverCurrentIrq() || pmu.isBatfetOverCurrentIrq())
            events |= BSP_POWER_EVENT_OVERCURRENT;
        if (pmu.isChargeOverTimeoutIrq())
            events |= BSP_POWER_EVENT_CHARGE_TIMEOUT;
        if (pmu.isPekeyShortPressIrq() || pmu.isPekeyLongPressIrq() || pmu.isPekeyNegativeIrq() ||
            pmu.isPekeyPositiveIrq())
            events |= BSP_POWER_EVENT_POWER_KEY;
        pmu.clearIrqStatus();
        if (events && s_callback)
            s_callback(events, s_callback_data);
    }
}

static esp_err_t init_irq() {
    const gpio_config_t config = {
        .pin_bit_mask = 1ULL << BSP_PMU_IRQ,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK)
        return err;
    err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
        return err;
    if (xTaskCreate(power_irq_task, "pmu_irq", 4096, nullptr, 8, &s_irq_task) != pdPASS)
        return ESP_ERR_NO_MEM;
    return gpio_isr_handler_add(BSP_PMU_IRQ, power_irq, nullptr);
}

esp_err_t bsp_power_init() {
    if (s_ready)
        return ESP_OK;
    esp_err_t err = bsp_i2c_init();
    if (err != ESP_OK)
        return err;
    err = s_pmu.init(BSP_I2C_AXP2101_ADDR, BSP_PMU_BOARD_TYPE, BSP_I2C_PORT);
    s_ready = err == ESP_OK;
    if (s_ready) {
        err = init_irq();
        if (err != ESP_OK)
            s_ready = false;
    }
    return err;
}

int bsp_power_battery_percent() { return s_ready ? s_pmu.getPmuBatteryPercent() : -1; }
bool bsp_power_battery_present() { return s_ready && s_pmu.getPmuBatteryConnect(); }

bool bsp_power_is_charging() {
    if (!s_ready)
        return false;
    const auto status = s_pmu.getPmuChargerStatus();
    return status == XPOWERS_AXP2101_CHG_PRE_STATE || status == XPOWERS_AXP2101_CHG_CC_STATE ||
           status == XPOWERS_AXP2101_CHG_CV_STATE;
}

esp_err_t bsp_power_get_status(bsp_power_status_t *status) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (!status)
        return ESP_ERR_INVALID_ARG;
    *status = {
        .battery_percent = s_pmu.getPmuBatteryPercent(),
        .battery_mv = pmu.getBattVoltage(),
        .system_mv = pmu.getSystemVoltage(),
        .vbus_mv = pmu.getVbusVoltage(),
        .battery_present = s_pmu.getPmuBatteryConnect(),
        .vbus_present = pmu.isVbusIn(),
        .charging = bsp_power_is_charging(),
        .power_on_source = static_cast<int>(s_pmu.getPmuPowerOnSource()),
        .power_off_source = static_cast<int>(s_pmu.getPmuPowerOffSource()),
    };
    return ESP_OK;
}

esp_err_t bsp_power_set_callback(bsp_power_callback_t callback, void *user_data) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_callback = callback;
    s_callback_data = user_data;
    return ESP_OK;
}

esp_err_t bsp_power_set_charging_enabled(bool enabled) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    if (enabled)
        s_pmu.enablePmuCellbatteryCharge();
    else
        s_pmu.disablePmuCellbatteryCharge();
    return ESP_OK;
}

esp_err_t bsp_power_set_wakeup_rails(bool enabled) {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    // magicbox_rtc_v1_5G: IO=ALDO4/3.3V, core=BLDO1/1.0V.
    // Generic VddIO/VddCore helpers target ALDO1/ALDO3 on a different PCB.
    if (enabled) {
        if (!s_pmu.setALDO4Voltage(3300) || !s_pmu.setBLDO1Voltage(1000) || !s_pmu.enableALDO4())
            return ESP_FAIL;
        vTaskDelay(pdMS_TO_TICKS(10));
        return s_pmu.enableBLDO1() ? ESP_OK : ESP_FAIL;
    }
    if (!s_pmu.disableBLDO1())
        return ESP_FAIL;
    vTaskDelay(pdMS_TO_TICKS(10));
    return s_pmu.disableALDO4() ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_power_enter_sleep() {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_pmu.pmu_enter_sleep();
    return ESP_OK;
}

esp_err_t bsp_power_shutdown() {
    if (!s_ready)
        return ESP_ERR_INVALID_STATE;
    s_pmu.pmu_shutdown();
    return ESP_OK;
}

AXP2101 *bsp_power_driver() { return s_ready ? &s_pmu : nullptr; }
