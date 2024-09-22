add_library(buttons_lib ./src/buttons.cpp)
target_include_directories(buttons_lib PUBLIC ./include)
target_link_libraries(buttons_lib
    PUBLIC
    pubsub_lib
    pico_stdlib)
