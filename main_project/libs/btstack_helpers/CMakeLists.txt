add_library(btstack_helpers_lib ./src/timer.cpp)
target_include_directories(btstack_helpers_lib PUBLIC ./include)
target_link_libraries(btstack_helpers_lib PUBLIC
    sg14_lib
    pico_stdlib
    bt_config_lib)
