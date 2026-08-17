cmake_minimum_required(VERSION 3.12)

add_library(usb)

target_sources(usb PRIVATE
    usb.c
    endpoint_0/handler.c
    endpoint_0/buffer.c
    endpoint_0/in.c
    endpoint_0/out.c
    endpoint_0/setup.c
    endpoint_0/transfer.c
)
