idf_component_register(
 SRCS "src/bsp_i2c.cpp" "src/bsp_power.cpp" "src/bsp_led.cpp"
      "src/bsp_audio.cpp" "src/bsp_rtc.cpp" "src/bsp_wakeup.cpp"
      "src/bsp_button.cpp" "src/bsp_knob.cpp"
 INCLUDE_DIRS "include"
 REQUIRES esp_adc esp_driver_gpio esp_driver_i2c esp_driver_i2s esp_driver_ledc esp_driver_spi esp_hw_support
          esp_codec_dev button led_strip folotoy__folotoy-firmware-driver)

# The pinned public driver predates IDF 6's removal of implicit driver exports.
# Adapt its build dependencies here without modifying downloaded vendor sources.
idf_component_get_property(folo_driver folotoy__folotoy-firmware-driver COMPONENT_LIB)
target_link_libraries(${folo_driver} PUBLIC idf::esp_driver_i2c idf::esp_driver_ledc idf::esp_driver_spi)
target_compile_options(${folo_driver} PRIVATE -Wno-missing-field-initializers)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
