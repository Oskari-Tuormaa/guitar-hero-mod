cmake_minimum_required(VERSION 3.20)

set(PICO_BOARD pico_w)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(PICO_DEOPTIMIZED_DEBUG 0)
set(CMAKE_BUILD_TYPE Release)

include(../3rdparty/pico-sdk/external/pico_sdk_import.cmake)
include(../3rdparty/pico-extras/external/pico_extras_import.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(guitar-hero-mod)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/bt.cpp
    src/hid.cpp
    src/buttons.cpp
    src/shutdown.cpp
    src/battery.cpp)

pico_btstack_make_gatt_header(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/kitara.gatt)

target_include_directories(${PROJECT_NAME} PUBLIC include)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_sleep
    hardware_adc)

pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC CYW43_LWIP=0)
