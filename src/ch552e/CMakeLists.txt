cmake_minimum_required(VERSION 3.12)

add_library(ch552e)

target_compile_definitions(ch552e PUBLIC
    F_SYS=${F_SYS}
)

target_sources(ch552e PRIVATE
    memory.c
)
