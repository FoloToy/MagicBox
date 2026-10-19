cmake_minimum_required(VERSION 3.16)

if(DEFINED IDF_TARGET AND NOT IDF_TARGET STREQUAL "esp32c5")
    message(FATAL_ERROR "This hardware profile requires ESP32-C5")
endif()
set(IDF_TARGET "esp32c5")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(magicbox-m301-c5)
