# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_protocol_SRCS)

include_directories(../common)
include_directories(../config)
include_directories(../../.)
include_directories(../mcp2515)
include_directories(../dynamixel)
include_directories(../icm42688)

# 生成链接库
add_library(protocol ${DIR_protocol_SRCS})
target_link_libraries(protocol PUBLIC pico_stdlib config mcp2515 dynamixel icm42688 first_order_filter)