#
# CMake CH552E (SDCC) toolchain
#
# 2026 @Enchan1207
#

# cross-compiling
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mcs51)

# commands
find_program(SDCC_EXECUTABLE sdcc REQUIRED)
set(CMAKE_C_COMPILER "${SDCC_EXECUTABLE}")

# Prevent CMake from trying to execute target binaries.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_OBJCOPY sdobjcopy)
# set(CMAKE_OBJDUMP sdobjdump) # command not found
set(CMAKE_NM sdnm)
set(CMAKE_AR sdar)
set(CMAKE_RANLIB sdranlib)
# set(CMAKE_STRIP sdstrip) # command not found

# Target configuration
set(SDCC_MODEL "small" CACHE STRING "SDCC memory model")
set(SDCC_XRAM_SIZE "0x0400" CACHE STRING "CH552 XRAM size")
set(SDCC_CODE_SIZE "0x3800" CACHE STRING "Application code size")

set(SDCC_COMMON_FLAGS
    "-mmcs51 --model-${SDCC_MODEL}"
)

set(CMAKE_C_FLAGS_INIT
    "${SDCC_COMMON_FLAGS}"
)

set(CMAKE_EXE_LINKER_FLAGS_INIT
    "${SDCC_COMMON_FLAGS} --xram-size ${SDCC_XRAM_SIZE} --code-size ${SDCC_CODE_SIZE}"
)

set(CMAKE_C_FLAGS_DEBUG_INIT
    "--debug"
)

set(CMAKE_C_FLAGS_RELEASE_INIT
    "--opt-code-size"
)
