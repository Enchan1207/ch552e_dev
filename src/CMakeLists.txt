cmake_minimum_required(VERSION 3.12)

add_subdirectory(hardware)
add_subdirectory(ch552e)

add_executable(ch552e_dev)

target_sources(ch552e_dev PRIVATE
    main.c
    setup_fifo.c
)

target_link_libraries(ch552e_dev PRIVATE 
    uart1
    usb
    ch552e
)
