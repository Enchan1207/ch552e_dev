cmake_minimum_required(VERSION 3.12)

project(ch552e_dev
    VERSION 1.0
    DESCRIPTION "CH552E development"
    LANGUAGES C ASM
)

set(F_SYS 6000000UL CACHE STRING "CH552 system clock frequency")

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(src)
