cmake_minimum_required(VERSION 3.12)

add_subdirectory(hardware)

add_executable(blink)

target_sources(blink PRIVATE
    main.c
)

target_link_libraries(blink PRIVATE 
    hardware
)
