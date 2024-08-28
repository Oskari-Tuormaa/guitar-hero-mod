cmake_minimum_required(VERSION 3.20)

set(PICO_BOARD pico_w)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(../../3rdparty/pico-sdk/pico_sdk_init.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(guitar_hid_test)

pico_sdk_init()

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME} pico_stdlib pico_btstack_classic
                      pico_btstack_cyw43 pico_cyw43_arch_none)
pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC CYW43_LWIP=0)
