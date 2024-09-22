add_library(shutdown_lib ./src/shutdown.cpp)
target_include_directories(shutdown_lib PUBLIC ./include)
target_link_libraries(shutdown_lib PUBLIC
    hardware_rosc
    hardware_sleep
    pubsub_lib
    bt_lib
    pico_stdlib)
