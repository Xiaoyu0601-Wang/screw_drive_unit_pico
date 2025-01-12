# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_Dynamixel_SRCS)

include_directories(../config)

# 生成链接库
add_library(dynamixel ${DIR_Dynamixel_SRCS})
target_link_libraries(dynamixel PUBLIC pico_stdlib config)