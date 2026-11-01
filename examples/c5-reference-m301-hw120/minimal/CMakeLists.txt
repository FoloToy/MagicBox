cmake_minimum_required(VERSION 3.16)

if(DEFINED IDF_TARGET AND NOT IDF_TARGET STREQUAL "esp32c5")
    message(FATAL_ERROR "This hardware profile requires ESP32-C5")
endif()
set(IDF_TARGET "esp32c5")
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../boards/c5-reference-m301-hw120/bsp")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(magicbox-c5)
