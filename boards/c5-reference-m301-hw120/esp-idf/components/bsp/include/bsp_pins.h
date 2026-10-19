// MagicBox C5 M301 HW V1.2.0. Source: product board config, not S3.
#pragma once
#include "driver/gpio.h"
#include "driver/i2c_types.h"
#include "driver/i2s_types.h"
#define BSP_I2C_PORT I2C_NUM_0
#define BSP_I2C_SDA GPIO_NUM_1
#define BSP_I2C_SCL GPIO_NUM_0
#define BSP_I2C_HZ 400000
#define BSP_I2C_AXP2101_ADDR 0x34
#define BSP_I2C_BM8563_ADDR 0x51
#define BSP_I2C_GX8002_ADDR 0x2F
#define BSP_I2C_ES8311_ADDR 0x18
#define BSP_PMU_IRQ GPIO_NUM_10
#define BSP_RTC_IRQ GPIO_NUM_8
#define BSP_GX8002_IRQ GPIO_NUM_7
#define BSP_LED_DATA GPIO_NUM_27
#define BSP_BUTTON_WIFI_GPIO GPIO_NUM_3
#define BSP_BUTTON_VOICE_GPIO GPIO_NUM_2
#define BSP_KNOB_GPIO GPIO_NUM_6
#define BSP_KNOB_ADC_UNIT ADC_UNIT_1
#define BSP_KNOB_ADC_CHANNEL ADC_CHANNEL_5
#define BSP_KNOB_RAW_MAX 4095
#define BSP_I2S_PORT I2S_NUM_0
#define BSP_I2S_MCLK GPIO_NUM_5
#define BSP_I2S_BCLK GPIO_NUM_25
#define BSP_I2S_WS GPIO_NUM_23
#define BSP_I2S_DOUT GPIO_NUM_4
#define BSP_I2S_DIN GPIO_NUM_24
#define BSP_AUDIO_PA GPIO_NUM_26
#define BSP_AUDIO_RATE 24000
#define BSP_PMU_TS_TYPE 0
#define BSP_PMU_BATTERY_TYPE 0
#define BSP_PMU_SLEEP_TYPE 2
#define BSP_PMU_POWER_SWITCH_TYPE 0
#define BSP_PMU_BOARD_TYPE                                                                         \
    ((BSP_PMU_TS_TYPE << 0) | (BSP_PMU_BATTERY_TYPE << 4) | (BSP_PMU_SLEEP_TYPE << 8) |            \
     (BSP_PMU_POWER_SWITCH_TYPE << 12))
