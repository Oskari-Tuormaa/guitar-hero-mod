add_library(bt_init_lib ./src/bt_init.cpp)

target_include_directories(bt_init_lib PUBLIC ./include)
target_link_libraries(bt_init_lib
    sg14_lib
    btstack_helpers_lib
    pico_stdlib
    pubsub_lib
    bt_config_lib)
