add_library(bt_config_lib INTERFACE)

target_include_directories(bt_config_lib INTERFACE ./include)
target_link_libraries(bt_config_lib INTERFACE
    pico_btstack_classic
    pico_btstack_cyw43
    pico_cyw43_arch_none)
