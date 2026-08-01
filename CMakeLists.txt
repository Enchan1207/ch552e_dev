cmake_minimum_required(VERSION 3.12)

project(ch_552e_blink
    VERSION 1.0
    DESCRIPTION "CH552E Blink Example"
    LANGUAGES C ASM
)

include_directories(BEFORE
    ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(src)
