cmake_minimum_required(VERSION 3.12)

add_library(hardware)

target_sources(hardware PRIVATE
    uart1.c
)
