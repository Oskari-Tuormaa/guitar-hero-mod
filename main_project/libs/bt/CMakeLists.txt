add_subdirectory(./bt_init)
add_subdirectory(./bt_config)
add_subdirectory(./btstack_helpers)
add_subdirectory(./bt_reconnect)

add_library(bt_lib INTERFACE)
target_link_libraries(bt_lib INTERFACE
    bt_init_lib
    bt_config_lib
    btstack_helpers_lib
    bt_reconnect_lib)
