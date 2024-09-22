add_library(bt_reconnect_lib src/bt_reconnect.cpp)

target_include_directories(bt_reconnect_lib PUBLIC ./include)
target_link_libraries(bt_reconnect_lib
    pico_stdlib
    pubsub_lib
    bt_init_lib)
