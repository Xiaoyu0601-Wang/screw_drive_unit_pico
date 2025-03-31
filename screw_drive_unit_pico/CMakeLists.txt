cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(screw_drive_unit_pico)
pico_sdk_init()

set(ROBOT_VERSION "v1_001")
set(UNIT_ID "1")

# 添加编译子目录
add_subdirectory(lib/common)
add_subdirectory(lib/config)
add_subdirectory(lib/icm42688)
add_subdirectory(lib/mcp2515)
add_subdirectory(lib/protocol)
add_subdirectory(lib/dynamixel)
add_subdirectory(lib/controller)
add_subdirectory(lib/imu)

# 添加头文件目录
include_directories(./individual_parameters/asr_sdm_${ROBOT_VERSION}/unit_${UNIT_ID})
include_directories(./lib/common)
include_directories(./lib/config)
include_directories(.)
include_directories(./lib/icm42688)
include_directories(./lib/mcp2515)
include_directories(./lib/protocol)
include_directories(./lib/dynamixel)
include_directories(./lib/controller)
include_directories(./lib/imu)

# 生成可执行文件
add_executable(main 
main.c
)

# enable usb output, disable uart output
# pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)

target_link_libraries(main pico_stdlib config icm42688 mcp2515 protocol controller fusion_ahrs)
