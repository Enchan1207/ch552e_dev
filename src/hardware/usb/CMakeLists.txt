cmake_minimum_required(VERSION 3.12)

add_library(usb)

target_sources(usb PRIVATE
    usb.c
    isr.c

    ep0/ep0.c
    ep0/setup.c
)
