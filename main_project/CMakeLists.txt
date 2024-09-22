cmake_minimum_required(VERSION 3.20)

set(PICO_BOARD pico_w)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

include(../3rdparty/pico-sdk/external/pico_sdk_import.cmake)
include(../3rdparty/pico-extras/external/pico_extras_import.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(guitar-hero-mod)

pico_sdk_init()

add_subdirectory(./libs)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME}
    bt_lib
    pubsub_lib
    parray_lib
    buttons_lib
    shutdown_lib
    pico_stdlib
    pico_btstack_classic
    pico_btstack_cyw43
    pico_cyw43_arch_none)
pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC CYW43_LWIP=0)
