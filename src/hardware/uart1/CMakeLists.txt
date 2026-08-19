cmake_minimum_required(VERSION 3.12)

add_library(uart1)

target_compile_definitions(uart1 PUBLIC
    F_SYS=${F_SYS}
)

target_sources(uart1 PRIVATE
    uart1.c
    isr.c
)
