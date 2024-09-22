add_library(pubsub_lib INTERFACE)
target_include_directories(pubsub_lib INTERFACE ./include)
target_link_libraries(pubsub_lib INTERFACE sg14_lib parray_lib)
