cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
project(screw_drive_unit_pico)
pico_sdk_init()

# 添加编译子目录
add_subdirectory(lib/Config)
add_subdirectory(lib/MCP2515)
add_subdirectory(lib/Protocol)
add_subdirectory(lib/Dynamixel)
add_subdirectory(lib/Controller)

# 添加头文件目录
include_directories(./lib/Config)
include_directories(./lib/MCP2515)
include_directories(./lib/Protocol)
include_directories(./lib/Dynamixel)
include_directories(./lib/Controller)

# 生成可执行文件
add_executable(main 
main.c
)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)

target_link_libraries(main pico_stdlib hardware_spi Config MCP2515 Protocol Controller)
