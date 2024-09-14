add_library(bt_lib ./src/bt.cpp)

target_include_directories(bt_lib PUBLIC ./include)
target_link_libraries(bt_lib
    sg14_lib
    btstack_helpers_lib
    pico_stdlib
    bt_config_lib)
