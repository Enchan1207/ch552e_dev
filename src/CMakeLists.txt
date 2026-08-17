cmake_minimum_required(VERSION 3.12)

add_subdirectory(hardware)
add_subdirectory(ch552e)

add_executable(blink)

target_sources(blink PRIVATE
    main.c
    func.c
)

target_link_libraries(blink PRIVATE 
    hardware
    ch552e
)
