# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_ePaper_SRCS 变量
aux_source_directory(. DIR_icm42688_SRCS)

include_directories(../config)
include_directories(../../.)
include_directories(../common)

# 生成链接库
add_library(icm42688 ${DIR_icm42688_SRCS})
target_link_libraries(icm42688 PUBLIC config first_order_filter)