cmake_minimum_required(VERSION 3.20)

set(PICO_BOARD pico_w)
set(CMAKE_CXX_STANDARD 20)

include(../../pico-sdk/pico_sdk_init.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 20)

project(bluetooth_hid_test)

pico_sdk_init()

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME} pico_stdlib pico_btstack_classic
                      pico_btstack_cyw43 pico_cyw43_arch_none)
pico_add_extra_outputs(${PROJECT_NAME})

# pico_btstack_make_gatt_header(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/bluetooth_hid_test.gatt")

target_compile_definitions(${PROJECT_NAME} PUBLIC CYW43_LWIP=0)
