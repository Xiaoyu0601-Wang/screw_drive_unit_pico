# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_Controller_SRCS)

include_directories(../Config)
include_directories(../Dynamixel)

# 生成链接库
add_library(Controller ${DIR_Controller_SRCS})
target_link_libraries(Controller PUBLIC pico_stdlib Config Dynamixel)