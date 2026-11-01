idf_component_register(SRCS "src/bsp.cpp" "src/bsp_audio.cpp"
 INCLUDE_DIRS "include"
 REQUIRES esp_adc esp_driver_gpio esp_driver_i2c esp_driver_i2s esp_hw_support led_strip)
