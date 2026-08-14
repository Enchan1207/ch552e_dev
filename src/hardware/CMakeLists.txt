cmake_minimum_required(VERSION 3.12)

add_library(hardware)

target_compile_definitions(hardware PUBLIC
    F_SYS=${F_SYS}
)

target_sources(hardware PRIVATE
    uart1.c
    usb/usb.c
)
