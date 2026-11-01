#pragma once

#include "bsp_pins.h"
#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <initializer_list>

// Owns the board I2C bus. Register transfers run only in the board worker task.
// GPIO14 is the active-low LIS3DH interrupt in the original HW2.1.0 design.
class MagicBoxMotion {
public:
  bool Begin() {
    i2c_master_bus_config_t bus = {};
    bus.i2c_port = I2C_NUM_0;
    bus.sda_io_num = static_cast<gpio_num_t>(BSP_I2C_SDA);
    bus.scl_io_num = static_cast<gpio_num_t>(BSP_I2C_SCL);
    bus.clk_source = I2C_CLK_SRC_DEFAULT;
    bus.glitch_ignore_cnt = 7;
    bus.flags.enable_internal_pullup = true;
    if (i2c_new_master_bus(&bus, &bus_) != ESP_OK)
      return false;
    i2c_device_config_t device = {};
    device.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    device.device_address = 0x19;
    device.scl_speed_hz = 100000;
    if (i2c_master_bus_add_device(bus_, &device, &device_) != ESP_OK)
      return false;
    uint8_t id = 0;
    if (!Read(0x0f, id) || id != 0x33)
      return false;
    // 50Hz low-power XYZ, +/-2g. High-pass motion detection, ~256mg,
    // 20ms duration, latched INT1 routed to both outputs, active-low.
    // CTRL6 mirrors the original board's INT2 routing/polarity.
    const uint8_t registers[][2] = {
        {0x20, 0x00}, {0x22, 0x00}, {0x25, 0x02}, {0x30, 0x00}, {0x21, 0x01},
        {0x23, 0x00}, {0x24, 0x08}, {0x32, 0x10}, {0x33, 0x01}, {0x20, 0x4f},
        {0x30, 0x2a}, {0x22, 0x40}, {0x25, 0x42}};
    for (const auto &entry : registers)
      if (!Write(entry[0], entry[1]))
        return false;
    uint8_t value;
    if (!Read(0x26, value) || !Read(0x31, value))
      return false;
    gpio_config_t io = {};
    io.pin_bit_mask = 1ULL << BSP_ACCEL_INT;
    io.mode = GPIO_MODE_INPUT;
    io.pull_up_en = GPIO_PULLUP_ENABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    if (gpio_config(&io) != ESP_OK)
      return false;
    // GPIO wake retains Wi-Fi association with automatic light sleep.
    for (int pin :
         {BSP_ACCEL_INT, BSP_KEYS[0], BSP_KEYS[1], BSP_EXTERNAL_MODE}) {
      if (gpio_wakeup_enable(static_cast<gpio_num_t>(pin),
                             GPIO_INTR_LOW_LEVEL) != ESP_OK)
        return false;
    }
    if (esp_sleep_enable_gpio_wakeup() != ESP_OK)
      return false;
    ready_ = true;
    ESP_LOGI("MagicBoxMotion",
             "LIS3DH id=0x33 motion interrupt GPIO14, threshold ~256mg");
    return true;
  }

  bool Poll() {
    // The sensor latches the event until read. GPIO wake needs no CPU ISR:
    // service the latched level in this worker, avoiding a level-IRQ storm.
    if (!ready_ || gpio_get_level(static_cast<gpio_num_t>(BSP_ACCEL_INT)))
      return false;
    uint8_t source = 0;
    if (!Read(0x31, source)) {
      if (++errors_ % 100 == 1)
        ESP_LOGW("MagicBoxMotion", "Cannot clear motion interrupt");
      return false;
    }
    if (source & 0x40) {
      ESP_LOGI("MagicBoxMotion", "Motion source=0x%02x GPIO14=%d", source,
               gpio_get_level(static_cast<gpio_num_t>(BSP_ACCEL_INT)));
      return true;
    }
    return false;
  }
  bool Ready() const { return ready_; }

private:
  i2c_master_bus_handle_t bus_ = nullptr;
  i2c_master_dev_handle_t device_ = nullptr;
  bool ready_ = false;
  unsigned errors_ = 0;
  bool Read(uint8_t reg, uint8_t &value) {
    return i2c_master_transmit_receive(device_, &reg, 1, &value, 1, 100) ==
           ESP_OK;
  }
  bool Write(uint8_t reg, uint8_t value) {
    const uint8_t bytes[] = {reg, value};
    return i2c_master_transmit(device_, bytes, 2, 100) == ESP_OK;
  }
};
