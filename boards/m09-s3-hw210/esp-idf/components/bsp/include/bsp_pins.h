#pragma once
// MagicBox M09 / ESP32-S3 / HW2.1.0. Original firmware framework: Arduino.
// A different S3 PCB must get a separate profile, never reuse these pins by chip alone.
constexpr int BSP_MIC_BCLK = 42, BSP_MIC_DATA = 2, BSP_MIC_WS = 1;
constexpr int BSP_SPEAKER_BCLK = 40, BSP_SPEAKER_DATA = 41, BSP_SPEAKER_WS = 39;
constexpr int BSP_VOLUME = 8, BSP_RGB = 46;
constexpr int BSP_KEYS[] = {4, 3};
// External Mode input, verified by GPIO6 edges and conversation transitions.
constexpr int BSP_EXTERNAL_MODE = 6;
constexpr int BSP_I2C_SDA = 12, BSP_I2C_SCL = 13, BSP_ACCEL_INT = 14;
constexpr const char *BSP_BOARD_NAME = "MagicBox HW v2.1.0 / ESP32-S3";
constexpr unsigned BSP_KEY_COUNT = sizeof(BSP_KEYS) / sizeof(BSP_KEYS[0]);
constexpr unsigned BSP_SAMPLE_RATE = 16000;
