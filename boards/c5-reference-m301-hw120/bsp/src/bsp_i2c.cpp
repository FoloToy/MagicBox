#include "bsp_i2c.h"
#include "bsp_pins.h"

#include "esp_log.h"
#include "port_i2c.hpp"

static const char *TAG = "bsp_i2c";
static i2c_master_bus_handle_t s_bus;

esp_err_t bsp_i2c_init(void) {
    if (s_bus)
        return ESP_OK;
    esp_err_t err = i2c_drv_init(BSP_I2C_PORT, BSP_I2C_SDA, BSP_I2C_SCL, BSP_I2C_HZ);
    if (err != ESP_OK)
        return err;
    err = i2c_master_get_bus_handle(BSP_I2C_PORT, &s_bus);
    if (err == ESP_OK)
        ESP_LOGI(TAG, "ready: SDA=GPIO%d SCL=GPIO%d", BSP_I2C_SDA, BSP_I2C_SCL);
    return err;
}

i2c_master_bus_handle_t bsp_i2c_bus(void) { return s_bus; }

esp_err_t bsp_i2c_scan(void) {
    if (!s_bus)
        return ESP_ERR_INVALID_STATE;
    int count = 0;
    for (uint8_t address = 0x08; address <= 0x77; ++address) {
        if (i2c_master_probe(s_bus, address, 50) == ESP_OK) {
            ESP_LOGI(TAG, "device at 0x%02X", address);
            ++count;
        }
    }
    ESP_LOGI(TAG, "scan complete: %d device(s)", count);
    return ESP_OK;
}
